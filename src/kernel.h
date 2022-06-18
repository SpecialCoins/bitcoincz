// Copyright (c) 2012-2013 The PPCoin developers
// Copyright (c) 2020 The BCZ Core Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_KERNEL_H
#define BITCOIN_KERNEL_H

#include "main.h"
#include "stakeinput.h"

uint256 ComputeStakeModifier(const CBlockIndex* pindexPrev, const uint256& kernel);
bool Stake(const CBlockIndex* pindexPrev, CStakeInput* stakeInput, unsigned int nBits, unsigned int& nTimeTx, uint256& hashProofOfStake);
bool initStakeInput(const CBlock block, std::unique_ptr<CStakeInput>& stake, int nPreviousBlockHeight);
bool CheckProofOfStake(const CBlock block, uint256& hashProofOfStake, std::unique_ptr<CStakeInput>& stake, int nPreviousBlockHeight);
bool CheckStakeKernelHash(const CBlockIndex* pindexPrev, const unsigned int nBits, CStakeInput* stake, const unsigned int nTimeTx, uint256& hashProofOfStake, const bool fVerify = false);
bool GetHashProofOfStake(const CBlockIndex* pindexPrev, CStakeInput* stake, const unsigned int nTimeTx, const bool fVerify, uint256& hashProofOfStakeRet);

#endif // BITCOIN_KERNEL_H
