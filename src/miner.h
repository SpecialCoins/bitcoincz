// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2016-2020 The BCZ developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_MINER_H
#define BITCOIN_MINER_H

#include "primitives/block.h"

#include <stdint.h>

class CBlock;
class CBlockHeader;
class CBlockIndex;
class COutput;
class CScript;
class CWallet;

static const bool DEFAULT_PRINTPRIORITY = false;

struct CBlockTemplate;

/** Generate a new block, without valid proof-of-work */
CBlockTemplate* CreateNewBlock(const CScript& scriptPubKeyIn, CWallet* pwallet, std::vector<COutput>* availableCoins = nullptr);
/** Modify the extranonce in a block */
void IncrementExtraNonce(CBlock* pblock, CBlockIndex* pindexPrev, unsigned int& nExtraNonce);
/** Check mined block */
void UpdateTime(CBlockHeader* block, const CBlockIndex* pindexPrev);

#ifdef ENABLE_WALLET
    void POSMiner(CWallet* pwallet);
    void StakeBCZ(bool fStake_BCZ, CWallet* pwallet);
#endif // ENABLE_WALLET

struct CBlockTemplate {
    CBlock block;
    std::vector<CAmount> vTxFees;
    std::vector<int64_t> vTxSigOps;
};

#endif // BITCOIN_MINER_H
