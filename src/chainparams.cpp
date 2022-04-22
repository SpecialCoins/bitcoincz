// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2020 The BCZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"

#include "chainparamsseeds.h"
#include "consensus/merkle.h"
#include "util.h"
#include "utilstrencodings.h"

#include <boost/assign/list_of.hpp>

#include <assert.h>

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 0x1e0fffff << CScriptNum(4)  << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.nVersion = nVersion;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "BCZ BORN";
    const CScript genesisOutputScript = CScript();
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
        (       0, uint256("0x00000fca55bd94508769f48d6d17c4aa3f9ae40b072036a3691752f1b70ef2dd"))
        (       1, uint256("0x000006bde18e8be09c47b5a76120be3cdc4661d935586a3ef730dbe67b3a70f0"))
        (      76, uint256("0x00000b5033e45adc8699cbc6b8240307f43104e04eb035115eba25fb047a9b4d"))
        (   51335, uint256("0x1a1c94116767503dbe26b657ec56ebc7948510c99de69ad73ac2355f649ce411"))
        (  108374, uint256("0x55957713ec6e6951e6f451c8cf3fb07c73cddaeb74dfefdb77acabba5d2c9f51"))
        (  162435, uint256("0xe33a7cbfdf66da89d490d0acf072e10fed16b8e6f7740061e64279534db0d36f"))
        (  190345, uint256("0xba5b39a88308941ea56e24daebac57e5e166a1593fda42279cff5f8509452d67"))
        (  576069, uint256("0x37954882f1e459aa3511eecea9ecf5cb49830c4d7a4f673a3f56775b2304a839"));

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1650103364, // * UNIX timestamp of last checkpoint block
    10000,    // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    500        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (0, uint256S("0x001"));

static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    0,
    0,
    0};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of
    (0, uint256S("0x001"));

static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    0,
    0,
    0};

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";

        genesis = CreateGenesisBlock(1554888888, 249257, 0x1e0fffff, 2, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00000fca55bd94508769f48d6d17c4aa3f9ae40b072036a3691752f1b70ef2dd"));
        assert(genesis.hashMerkleRoot == uint256S("0x0adff40a2ade8baf5c2c311748928ff4d4d727d2042b1f685fe8ca1f70a1021b"));
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.powLimit   = ~UINT256_ZERO >> 20;
        bnProofOfWorkLimit = ~uint256(0) >> 20;
        consensus.nCoinbaseMaturity = 100;
        consensus.nMaxMoneyOut = 99000000 * COIN;
        consensus.nMNCollateralAmt = 5000 * COIN;
        consensus.nPoolMaxTransactions = 3;
        consensus.nStakeMinDepth = 120;
        consensus.nTargetTimespan = 150;
        consensus.nTargetSpacing = 150;
        consensus.nTimeSlotLength = 180;

        // spork keys
        consensus.strSporkPubKey = "04d7437801b20f7d3e585e829088d0773846b80fd06788d6f74b65d258b03fe9cbc2d2fb355bd878f81c491db08fbe5556d6aa220a80b43be927f4bdb41d2d00e8";

        // Network upgrades
        consensus.vUpgrades[Consensus::BASE_NETWORK].nActivationHeight =
                Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
                Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_POS].nActivationHeight           = 39125;
        consensus.vUpgrades[Consensus::UPGRADE_POW_END].nActivationHeight       = 50000;
        consensus.vUpgrades[Consensus::UPGRADE_MODV2].nActivationHeight         = 66555;
        consensus.vUpgrades[Consensus::UPGRADE_V5_DUMMY].nActivationHeight =
                Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        pchMessageStart[0] = { 'b' };
        pchMessageStart[1] = { 'c' };
        pchMessageStart[2] = { 'z' };
        pchMessageStart[3] = { '0' };
        nDefaultPort = 29500;
        vSeeds.push_back(CDNSSeedData("51.91.156.249", "51.91.156.249"));
        vSeeds.push_back(CDNSSeedData("51.91.156.251", "51.91.156.251"));
        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 25);
        base58Prefixes[STAKING_ADDRESS] = std::vector<unsigned char>(1, 63);     // starting with 'S'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 20);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 210);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x04)(0x88)(0xB2)(0x1E).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x04)(0x88)(0xAD)(0xE4).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();
        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }

};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }

    void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
    {
        assert(idx > Consensus::BASE_NETWORK && idx < Consensus::MAX_NETWORK_UPGRADES);
        consensus.vUpgrades[idx].nActivationHeight = nActivationHeight;
    }
};
static CRegTestParams regTestParams;

static CChainParams* pCurrentParams = 0;

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}

void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
{
    regTestParams.UpdateNetworkUpgradeParameters(idx, nActivationHeight);
}
