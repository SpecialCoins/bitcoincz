// Copyright (c) 2011-2013 The PPCoin developers
// Copyright (c) 2013-2014 The NovaCoin Developers
// Copyright (c) 2014-2018 The BlackCoin Developers
// Copyright (c) 2015-2020 The BCZ developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BCZ_KERNEL_H
#define BCZ_KERNEL_H

#include "main.h"
#include "stakeinput.h"

class CStakeKernel {
public:
    /**
     * CStakeKernel Constructor
     *
     * @param[in]   pindexPrev      index of the parent of the kernel block
     * @param[in]   stakeInput      input for the coinstake of the kernel block
     * @param[in]   nBits           target difficulty bits of the kernel block
     * @param[in]   nTimeTx         time of the kernel block
     */
    CStakeKernel(const CBlockIndex* const pindexPrev, CStakeInput* stakeInput, unsigned int nBits, int nTimeTx);

    // Return stake kernel hash
    uint256 GetHash() const;

    // Check that the kernel hash meets the target required
    bool CheckKernelHash(bool fSkipLog = false) const;

private:
    // kernel message hashed
    CDataStream stakeModifier{CDataStream(SER_GETHASH, 0)};
    int nTimeBlockFrom{0};
    CDataStream stakeUniqueness{CDataStream(SER_GETHASH, 0)};
    int nTime{0};
    // hash target
    unsigned int nBits{0};     // difficulty for the target
    CAmount stakeValue{0};     // target multiplier
};

bool Stake(const CBlockIndex* pindexPrev, CStakeInput* stakeInput, unsigned int nBits, int64_t& nTimeTx);
bool CheckProofOfStake(const CBlock& block, std::string& strError, const CBlockIndex* pindexPrev = nullptr);
bool GetStakeKernelHash(uint256& hashRet, const CBlock& block, const CBlockIndex* pindexPrev = nullptr);

#endif // BCZ_KERNEL_H
