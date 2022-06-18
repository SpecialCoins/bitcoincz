// Copyright (c) 2012-2013 The PPCoin developers
// Copyright (c) 2020 The BCZ Core Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp>

#include "db.h"
#include "kernel.h"
#include "script/interpreter.h"
#include "timedata.h"
#include "util.h"
#include "utilmoneystr.h"
#include "stakeinput.h"
#include "spork.h"
#include "masternode-sync.h"

uint256 ComputeStakeModifier(const CBlockIndex* pindexPrev, const uint256& kernel)
{
    if (!pindexPrev)
        return uint256(); // genesis block's modifier is 0

    CHashWriter ss(SER_GETHASH, 0);
    ss << kernel;
    ss << pindexPrev->nStakeModifierV2;

    return ss.GetHash();
}

bool CheckStakeKernelHash(const CBlockIndex* pindexPrev, const unsigned int nBits, CStakeInput* stake, const unsigned int nTimeTx, uint256& hashProofOfStake, const bool fVerify)
{
    // Calculate the proof of stake hash
    if (!GetHashProofOfStake(pindexPrev, stake, nTimeTx, fVerify, hashProofOfStake)) {
        return error("%s : Failed to calculate the proof of stake hash", __func__);
    }

    const CAmount& nValueIn = stake->GetValue();
    const CDataStream& ssUniqueID = stake->GetUniqueness();

    if ((nValueIn < 100 * COIN) && masternodeSync.IsBlockchainSynced())
    {
        return false;
    }

    // Base target
    uint256 bnTarget;
    bnTarget.SetCompact(nBits);

    // Weighted target
    uint256 bnWeight = uint256(nValueIn) / 100;
    bnTarget *= bnWeight;

    // Check if proof-of-stake hash meets target protocol
    const bool res = (hashProofOfStake < bnTarget);

    if (fVerify || res) {
        LogPrintf("staking", "%s : Proof Of Stake:"
                            "\nssUniqueID=%s"
                            "\nnTimeTx=%d"
                            "\nhashProofOfStake=%s"
                            "\nnBits=%d"
                            "\nweight=%d"
                            "\nbnTarget=%s (res: %d)\n\n",
            __func__, HexStr(ssUniqueID), nTimeTx, hashProofOfStake.GetHex(),
            nBits, nValueIn, bnTarget.GetHex(), res);
    }
    return res;
}

bool GetHashProofOfStake(const CBlockIndex* pindexPrev, CStakeInput* stake, const unsigned int nTimeTx, const bool fVerify, uint256& hashProofOfStakeRet) {
    // Grab the stake data
    CBlockIndex* pindexfrom = stake->GetIndexFrom();
    if (!pindexfrom) return error("%s : Failed to find the block index for stake origin", __func__);
    const CDataStream& ssUniqueID = stake->GetUniqueness();
    const unsigned int nTimeBlockFrom = pindexfrom->nTime;
    CDataStream modifier_ss(SER_GETHASH, 0);
    modifier_ss << pindexPrev->nStakeModifierV2;

    CDataStream ss(modifier_ss);
    // Calculate hash
    ss << nTimeBlockFrom << ssUniqueID << nTimeTx;
    hashProofOfStakeRet = Hash(ss.begin(), ss.end());

    return true;
}

bool Stake(const CBlockIndex* pindexPrev, CStakeInput* stakeInput, unsigned int nBits, unsigned int& nTimeTx, uint256& hashProofOfStake)
{
    int prevHeight = pindexPrev->nHeight;

    // get stake input pindex
    CBlockIndex* pindexFrom = stakeInput->GetIndexFrom();
    if (!pindexFrom || pindexFrom->nHeight < 1) return error("%s : no pindexfrom", __func__);

    const uint32_t nTimeBlockFrom = pindexFrom->nTime;
    const int nHeightBlockFrom = pindexFrom->nHeight;

    // check for maturity (min age/depth) requirements
    if (!Params().HasStakeMinAgeOrDepth(prevHeight + 1, nTimeTx, nHeightBlockFrom, nTimeBlockFrom))
        return error("%s : min age violation - height=%d - nTimeTx=%d, nTimeBlockFrom=%d, nHeightBlockFrom=%d",
                         __func__, prevHeight + 1, nTimeTx, nTimeBlockFrom, nHeightBlockFrom);

    // iterate the hashing
    bool fSuccess = false;
    const unsigned int nHashDrift = 60;
    unsigned int nTryTime = nTimeTx - 1;
    // iterate from nTimeTx up to nTimeTx + nHashDrift
    // but not after the max allowed future blocktime drift (3 minutes for PoS)
    const unsigned int maxTime = std::min(nTimeTx + nHashDrift, Params().MaxFutureTime(GetAdjustedTime()));

    while (nTryTime < maxTime)
    {
        //new block came in, move on
        if (chainActive.Height() != prevHeight)
            break;

        ++nTryTime;

        // if stake hash does not meet the target then continue to next iteration
        if (!CheckStakeKernelHash(pindexPrev, nBits, stakeInput, nTryTime, hashProofOfStake))
            continue;

        // if we made it this far, then we have successfully found a valid kernel hash
        fSuccess = true;
        nTimeTx = nTryTime;
        break;
    }

    mapHashedBlocks.clear();
    mapHashedBlocks[chainActive.Tip()->nHeight] = GetTime(); //store a time stamp of when we last hashed on this block
    return fSuccess;
}

bool initStakeInput(const CBlock block, std::unique_ptr<CStakeInput>& stake, int nPreviousBlockHeight) {
    const CTransaction tx = block.vtx[1];
    if (!tx.IsCoinStake())
        return error("%s : called on non-coinstake %s", __func__, tx.GetHash().ToString().c_str());

    // Kernel (input 0) must match the stake hash target per coin age (nBits)
    const CTxIn& txin = tx.vin[0];

    //Construct the stakeinput object
        // First try finding the previous transaction in database
        uint256 hashBlock;
        CTransaction txPrev;
        if (!GetTransaction(txin.prevout.hash, txPrev, hashBlock, true))
            return error("%s : INFO: read txPrev failed, tx id prev: %s, block id %s",
                         __func__, txin.prevout.hash.GetHex(), block.GetHash().GetHex());

        //verify signature and script
        if (!VerifyScript(txin.scriptSig, txPrev.vout[txin.prevout.n].scriptPubKey, STANDARD_SCRIPT_VERIFY_FLAGS, TransactionSignatureChecker(&tx, 0)))
            return error("%s : VerifySignature failed on coinstake %s", __func__, tx.GetHash().ToString().c_str());

        CBczStake* bczInput = new CBczStake();
        bczInput->SetPrevout(txPrev, txin.prevout.n);
        stake = std::unique_ptr<CStakeInput>(bczInput);

    return true;
}

// Check kernel hash target and coinstake signature
bool CheckProofOfStake(const CBlock block, uint256& hashProofOfStake, std::unique_ptr<CStakeInput>& stake, int nPreviousBlockHeight)
{
    // Initialize the stake object
    if(!initStakeInput(block, stake, nPreviousBlockHeight))
        return error("%s : stake input object initialization failed", __func__);

    const CTransaction tx = block.vtx[1];
    // Kernel (input 0) must match the stake hash target per coin age (nBits)
    const CTxIn& txin = tx.vin[0];
    CBlockIndex* pindexPrev = mapBlockIndex[block.hashPrevBlock];
    CBlockIndex* pindexfrom = stake->GetIndexFrom();
    if (!pindexfrom)
        return error("%s : Failed to find the block index for stake origin", __func__);

    unsigned int nBlockFromTime = pindexfrom->nTime;
    unsigned int nTxTime = block.nTime;
    const int nBlockFromHeight = pindexfrom->nHeight;

    if (nPreviousBlockHeight >= 1) {
        //check for maturity (min age/depth) requirements
        if (!Params().HasStakeMinAgeOrDepth(nPreviousBlockHeight+1, nTxTime, nBlockFromHeight, nBlockFromTime))
            return error("%s : min age violation - height=%d - nTimeTx=%d, nTimeBlockFrom=%d, nHeightBlockFrom=%d",
                             __func__, nPreviousBlockHeight, nTxTime, nBlockFromTime, nBlockFromHeight);
    }

    if (!CheckStakeKernelHash(pindexPrev, block.nBits, stake.get(), nTxTime, hashProofOfStake, true))
        return error("%s : INFO: check kernel failed on coinstake %s, hashProof=%s", __func__,
                     tx.GetHash().GetHex(), hashProofOfStake.GetHex());

    return true;
}
