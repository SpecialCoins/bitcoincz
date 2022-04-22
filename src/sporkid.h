// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2020 The BCZ developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef SPORKID_H
#define SPORKID_H

/*
    Don't ever reuse these IDs for other sporks
    - This would result in old clients getting confused about which spork is for what
*/

enum SporkId : int32_t {
    SPORK_2_SWIFTTX                             = 10002,
    SPORK_3_SWIFTTX_BLOCK_FILTERING             = 10003,
    SPORK_5_MAX_VALUE                           = 10005,
    SPORK_7_SAPLING_MAINTENANCE                 = 10007,
    SPORK_8_LEGACY_MNS_MAX_HEIGHT               = 10008,
    SPORK_9_LLMQ_DKG_MAINTENANCE                = 10009,
    SPORK_14_NEW_PROTOCOL_ENFORCEMENT           = 10013,
    SPORK_15_NEW_PROTOCOL_ENFORCEMENT_2         = 10014,
    SPORK_20_ZERO_FEES                          = 10020,
    SPORK_21_MASTERNODE_PAYMENT_ENFORCEMENT     = 10021,
    SPORK_22_MASTERNODE_PAYMENT                 = 10022,
    SPORK_23_F_PAYMENT                          = 10023,
    SPORK_24_F_PAYMENT_ENFORCEMENT              = 10024,
    SPORK_26_COLDSTAKING_MAINTENANCE            = 10006,
    SPORK_30_SAFETY_KILL                        = 10030,
    SPORK_31_SAFETY_KILL_NEW                    = 10031,
    SPORK_32_F_PAYMENT_NEW                      = 10032,
    SPORK_33_FREE_OUT                           = 10033,
    SPORK_INVALID                               = -1
};

// Default values
struct CSporkDef
{
    CSporkDef(): sporkId(SPORK_INVALID), defaultValue(0) {}
    CSporkDef(SporkId id, int64_t val, std::string n): sporkId(id), defaultValue(val), name(n) {}
    SporkId sporkId;
    int64_t defaultValue;
    std::string name;
};

#endif
