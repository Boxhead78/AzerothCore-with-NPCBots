/*
 * Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "ObjectMgr.h"
#include "AuctionHouseMgr.h"
#include "AuctionHouseSearcher.h"
#include "AuctionHouseBot.h"
#include "Config.h"
#include "Player.h"
#include "WorldSession.h"
#include "GameTime.h"
#include <vector>

using namespace std;
vector<uint32> npcItems;
vector<uint32> lootItems;
vector<uint32> greyTradeGoodsBin;
vector<uint32> whiteTradeGoodsBin;
vector<uint32> greenTradeGoodsBin;
vector<uint32> blueTradeGoodsBin;
vector<uint32> purpleTradeGoodsBin;
vector<uint32> orangeTradeGoodsBin;
vector<uint32> yellowTradeGoodsBin;
vector<uint32> greyItemsBin;
vector<uint32> whiteItemsBin;
vector<uint32> greenItemsBin;
vector<uint32> blueItemsBin;
vector<uint32> purpleItemsBin;
vector<uint32> orangeItemsBin;
vector<uint32> yellowItemsBin;

AuctionHouseBot::AuctionHouseBot()
{
    debug_Out = false;
    debug_Out_Filters = false;
    AHBSeller = false;
    AHBBuyer = false;

    //Begin Filters

    Vendor_Items = false;
    Loot_Items = false;
    Other_Items = false;
    Vendor_TGs = false;
    Loot_TGs = false;
    Other_TGs = false;

    No_Bind = false;
    Bind_When_Picked_Up = false;
    Bind_When_Equipped = false;
    Bind_When_Use = false;
    Bind_Quest_Item = false;

    DisablePermEnchant = false;
    DisableConjured = false;
    DisableGems = false;
    DisableMoney = false;
    DisableMoneyLoot = false;
    DisableLootable = false;
    DisableKeys = false;
    DisableDuration = false;
    DisableBOP_Or_Quest_NoReqLevel = false;
    DisablePets = false;
    DisableMounts = false;

    DisableWarriorItems = false;
    DisablePaladinItems = false;
    DisableHunterItems = false;
    DisableRogueItems = false;
    DisablePriestItems = false;
    DisableDKItems = false;
    DisableShamanItems = false;
    DisableMageItems = false;
    DisableWarlockItems = false;
    DisableUnusedClassItems = false;
    DisableDruidItems = false;

    DisableItemsBelowLevel = 0;
    DisableItemsAboveLevel = 0;
    DisableTGsBelowLevel = 0;
    DisableTGsAboveLevel = 0;
    DisableItemsBelowGUID = 0;
    DisableItemsAboveGUID = 0;
    DisableTGsBelowGUID = 0;
    DisableTGsAboveGUID = 0;
    DisableItemsBelowReqLevel = 0;
    DisableItemsAboveReqLevel = 0;
    DisableTGsBelowReqLevel = 0;
    DisableTGsAboveReqLevel = 0;
    DisableItemsBelowReqSkillRank = 0;
    DisableItemsAboveReqSkillRank = 0;
    DisableTGsBelowReqSkillRank = 0;
    DisableTGsAboveReqSkillRank = 0;

    //End Filters

    _lastrun_a = time(nullptr);
    _lastrun_h = time(nullptr);
    _lastrun_n = time(nullptr);

    AllianceConfig = AHBConfig(2);
    HordeConfig = AHBConfig(6);
    NeutralConfig = AHBConfig(7);
}

AuctionHouseBot::~AuctionHouseBot()
{
}

void AuctionHouseBot::addNewAuctions(Player* AHBplayer, AHBConfig* config)
{
    if (!AHBSeller)
    {
        if (debug_Out)
            LOG_INFO("module", "AHSeller: Disabled");
        return;
    }

    uint32 minItems = config->GetMinItems();
    uint32 maxItems = config->GetMaxItems();

    if (maxItems == 0)
    {
        return;
    }

    AuctionHouseEntry const* ahEntry = sAuctionMgr->GetAuctionHouseEntryFromFactionTemplate(config->GetAHFID());
    if (!ahEntry)
    {
        return;
    }
    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());
    if (!auctionHouse)
    {
        return;
    }

    uint32 auctions = auctionHouse->Getcount();

    uint32 items = 0;

    if (auctions >= minItems)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHSeller: Auctions above minimum");
        return;
    }

    if (auctions >= maxItems)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHSeller: Auctions at or above maximum");
        return;
    }

    if ((maxItems - auctions) >= ItemsPerCycle)
        items = ItemsPerCycle;
    else
        items = (maxItems - auctions);

    if (debug_Out)
        LOG_INFO("module", "AHSeller: Adding {} Auctions", items);

    if (debug_Out)
        LOG_ERROR("module", "AHSeller: Current house id is {}", config->GetAHID());

    uint32 greyTGcount = config->GetPercents(AHB_GREY_TG);
    uint32 whiteTGcount = config->GetPercents(AHB_WHITE_TG);
    uint32 greenTGcount = config->GetPercents(AHB_GREEN_TG);
    uint32 blueTGcount = config->GetPercents(AHB_BLUE_TG);
    uint32 purpleTGcount = config->GetPercents(AHB_PURPLE_TG);
    uint32 orangeTGcount = config->GetPercents(AHB_ORANGE_TG);
    uint32 yellowTGcount = config->GetPercents(AHB_YELLOW_TG);
    uint32 greyIcount = config->GetPercents(AHB_GREY_I);
    uint32 whiteIcount = config->GetPercents(AHB_WHITE_I);
    uint32 greenIcount = config->GetPercents(AHB_GREEN_I);
    uint32 blueIcount = config->GetPercents(AHB_BLUE_I);
    uint32 purpleIcount = config->GetPercents(AHB_PURPLE_I);
    uint32 orangeIcount = config->GetPercents(AHB_ORANGE_I);
    uint32 yellowIcount = config->GetPercents(AHB_YELLOW_I);
    uint32 greyTGoods = config->GetItemCounts(AHB_GREY_TG);
    uint32 whiteTGoods = config->GetItemCounts(AHB_WHITE_TG);
    uint32 greenTGoods = config->GetItemCounts(AHB_GREEN_TG);
    uint32 blueTGoods = config->GetItemCounts(AHB_BLUE_TG);
    uint32 purpleTGoods = config->GetItemCounts(AHB_PURPLE_TG);
    uint32 orangeTGoods = config->GetItemCounts(AHB_ORANGE_TG);
    uint32 yellowTGoods = config->GetItemCounts(AHB_YELLOW_TG);

    uint32 greyItems = config->GetItemCounts(AHB_GREY_I);
    uint32 whiteItems = config->GetItemCounts(AHB_WHITE_I);
    uint32 greenItems = config->GetItemCounts(AHB_GREEN_I);
    uint32 blueItems = config->GetItemCounts(AHB_BLUE_I);
    uint32 purpleItems = config->GetItemCounts(AHB_PURPLE_I);
    uint32 orangeItems = config->GetItemCounts(AHB_ORANGE_I);
    uint32 yellowItems = config->GetItemCounts(AHB_YELLOW_I);

    if (debug_Out)
        LOG_ERROR("module", "AHSeller: {} items", items);

    auto trans = CharacterDatabase.BeginTransaction();
    // only insert a few at a time, so as not to peg the processor
    for (uint32 cnt = 1; cnt <= items; cnt++)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHSeller: {} count", cnt);

        uint32 itemID = 0;
        uint32 itemColor = 99;
        uint32 loopbreaker = 0;
        while (itemID == 0 && loopbreaker <= 50)
        {
            ++loopbreaker;
            uint32 choice = urand(0, 13);
            itemColor = choice;
            switch (choice)
            {
                case 0:
                {
                    if ((!greyItemsBin.empty()) && (greyItems < greyIcount))
                        itemID = greyItemsBin[urand(0, greyItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 1:
                {
                    if ((!whiteItemsBin.empty()) && (whiteItems < whiteIcount))
                        itemID = whiteItemsBin[urand(0, whiteItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 2:
                {
                    if ((!greenItemsBin.empty()) && (greenItems < greenIcount))
                        itemID = greenItemsBin[urand(0, greenItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 3:
                {
                    if ((!blueItemsBin.empty()) && (blueItems < blueIcount))
                        itemID = blueItemsBin[urand(0, blueItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 4:
                {
                    if ((!purpleItemsBin.empty()) && (purpleItems < purpleIcount))
                        itemID = purpleItemsBin[urand(0, purpleItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 5:
                {
                    if ((!orangeItemsBin.empty()) && (orangeItems < orangeIcount))
                        itemID = orangeItemsBin[urand(0, orangeItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 6:
                {
                    if ((!yellowItemsBin.empty()) && (yellowItems < yellowIcount))
                        itemID = yellowItemsBin[urand(0, yellowItemsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 7:
                {
                    if ((!greyTradeGoodsBin.empty()) && (greyTGoods < greyTGcount))
                        itemID = greyTradeGoodsBin[urand(0, greyTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 8:
                {
                    if ((!whiteTradeGoodsBin.empty()) && (whiteTGoods < whiteTGcount))
                        itemID = whiteTradeGoodsBin[urand(0, whiteTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 9:
                {
                    if ((!greenTradeGoodsBin.empty()) && (greenTGoods < greenTGcount))
                        itemID = greenTradeGoodsBin[urand(0, greenTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 10:
                {
                    if ((!blueTradeGoodsBin.empty()) && (blueTGoods < blueTGcount))
                        itemID = blueTradeGoodsBin[urand(0, blueTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 11:
                {
                    if ((!purpleTradeGoodsBin.empty()) && (purpleTGoods < purpleTGcount))
                        itemID = purpleTradeGoodsBin[urand(0, purpleTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 12:
                {
                    if ((!orangeTradeGoodsBin.empty()) && (orangeTGoods < orangeTGcount))
                        itemID = orangeTradeGoodsBin[urand(0, orangeTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                case 13:
                {
                    if ((!yellowTradeGoodsBin.empty()) && (yellowTGoods < yellowTGcount))
                        itemID = yellowTradeGoodsBin[urand(0, yellowTradeGoodsBin.size() - 1)];
                    else continue;
                    break;
                }
                default:
                {
                    if (debug_Out)
                        LOG_ERROR("module", "AHSeller: itemID Switch - Default Reached");
                    break;
                }
            }

            if (itemID == 0)
            {
                if (debug_Out)
                    LOG_ERROR("module", "AHSeller: Item::CreateItem() - ItemID is 0");
                continue;
            }

            ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(itemID);
            if (prototype == nullptr)
            {
                if (debug_Out)
                    LOG_ERROR("module", "AHSeller: Huh?!?! prototype == NULL");
                continue;
            }

            Item* item = Item::CreateItem(itemID, 1, AHBplayer);
            if (item == nullptr)
            {
                if (debug_Out)
                    LOG_ERROR("module", "AHSeller: Item::CreateItem() returned NULL");
                break;
            }
            item->AddToUpdateQueueOf(AHBplayer);

            uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(itemID);
            if (randomPropertyId != 0)
                item->SetItemRandomProperties(randomPropertyId);

            uint64 buyoutPrice = getPrice(prototype, UseBuyPriceForSeller, true) * getCustomScaling(prototype) / 100;
            uint64 bidPrice = 0;
            uint32 stackCount = 1;

            if (prototype->Quality <= AHB_MAX_QUALITY)
            {
                stackCount = getMaxStackCount(config, item, prototype);
                buyoutPrice *= urand(config->GetMinPrice(prototype->Quality), config->GetMaxPrice(prototype->Quality));
                buyoutPrice /= 100;
                bidPrice = buyoutPrice * urand(config->GetMinBidPrice(prototype->Quality), config->GetMaxBidPrice(prototype->Quality));
                bidPrice /= 100;
            } else
            {
                // quality is something it shouldn't be, let's get out of here
                if (debug_Out)
                    LOG_ERROR("module", "AHBuyer: Quality {} not Supported", prototype->Quality);
                item->RemoveFromUpdateQueueOf(AHBplayer);
                continue;
            }

            uint32 etime = urand(1, 3);
            switch (etime)
            {
                case 1:
                    etime = 43200;
                    break;
                case 2:
                    etime = 86400;
                    break;
                case 3:
                    etime = 172800;
                    break;
                default:
                    etime = 86400;
                    break;
            }
            item->SetCount(stackCount);

            uint32 dep = sAuctionMgr->GetAuctionDeposit(ahEntry, etime, item, stackCount);

            auto* auctionEntry = new AuctionEntry();
            auctionEntry->Id = sObjectMgr->GenerateAuctionID();
            auctionEntry->houseId = AuctionHouseId(config->GetAHID());
            auctionEntry->item_guid = item->GetGUID();
            auctionEntry->item_template = item->GetEntry();
            auctionEntry->itemCount = item->GetCount();
            auctionEntry->owner = AHBplayer->GetGUID();
            auctionEntry->startbid = bidPrice * stackCount;
            auctionEntry->buyout = buyoutPrice * stackCount;
            auctionEntry->bid = 0;
            auctionEntry->deposit = dep;
            auctionEntry->expire_time = (time_t) etime + time(nullptr);
            auctionEntry->auctionHouseEntry = ahEntry;
            item->SaveToDB(trans);
            item->RemoveFromUpdateQueueOf(AHBplayer);
            sAuctionMgr->AddAItem(item);
            auctionHouse->AddAuction(auctionEntry);
            auctionEntry->SaveToDB(trans);

            switch (itemColor)
            {
                case 0:
                    ++greyItems;
                    break;
                case 1:
                    ++whiteItems;
                    break;
                case 2:
                    ++greenItems;
                    break;
                case 3:
                    ++blueItems;
                    break;
                case 4:
                    ++purpleItems;
                    break;
                case 5:
                    ++orangeItems;
                    break;
                case 6:
                    ++yellowItems;
                    break;
                case 7:
                    ++greyTGoods;
                    break;
                case 8:
                    ++whiteTGoods;
                    break;
                case 9:
                    ++greenTGoods;
                    break;
                case 10:
                    ++blueTGoods;
                    break;
                case 11:
                    ++purpleTGoods;
                    break;
                case 12:
                    ++orangeTGoods;
                    break;
                case 13:
                    ++yellowTGoods;
                    break;
                default:
                    break;
            }
        }
    }
    CharacterDatabase.CommitTransaction(trans);
}

uint32 AuctionHouseBot::getMaxStackCount(AHBConfig* config, const Item* item, const ItemTemplate* prototype)
{
    if (item->GetMaxStackCount() == 1)
    {
        return 1;
    }

    if (prototype->Class == ITEM_CLASS_GLYPH)
    {
        return 1;
    }

    if (prototype->Class == ITEM_CLASS_CONSUMABLE && prototype->SubClass == ITEM_SUBCLASS_ITEM_ENHANCEMENT)
    {
        return 1;
    }

    if (prototype->Class == ITEM_CLASS_QUEST)
    {
        return 1;
    }

    if (config->GetMaxStack(prototype->Quality) > 1)
    {
       return urand(1, minValue(item->GetMaxStackCount(), config->GetMaxStack(prototype->Quality)));
    }

    return urand(1, item->GetMaxStackCount());
}

void AuctionHouseBot::addNewAuctionBuyerBotBid(Player* AHBplayer, AHBConfig* config, WorldSession* session)
{
    if (!AHBBuyer)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBuyer: Disabled");
        return;
    }

    QueryResult result = CharacterDatabase.Query("SELECT id FROM auctionhouse WHERE itemowner<>{} AND buyguid<>{}", AHBplayerGUID, AHBplayerGUID);

    if (!result)
        return;

    if (result->GetRowCount() == 0)
        return;

    // Fetches content of selected AH
    AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());
    vector<uint32> possibleBids;

    do
    {
        uint32 tmpdata = result->Fetch()->Get<uint32>();
        possibleBids.push_back(tmpdata);
    } while (result->NextRow());

    for (uint32 count = 1; count <= config->GetBidsPerInterval(); ++count)
    {
        // Do we have anything to bid? If not, stop here.
        if (possibleBids.empty())
        {
            //if (debug_Out) sLog->outError( "AHBuyer: I have no items to bid on.");
            count = config->GetBidsPerInterval();
            continue;
        }

        // Choose random auction from possible auctions
        uint32 vectorPos = urand(0, possibleBids.size() - 1);
        std::vector<uint32>::iterator iter = possibleBids.begin();
        advance(iter, vectorPos);

        // from auctionhousehandler.cpp, creates auction pointer & player pointer
        AuctionEntry* auction = auctionHouse->GetAuction(*iter);

        // Erase the auction from the vector to prevent bidding on item in next iteration.
        possibleBids.erase(iter);

        if (!auction)
            continue;

        // get exact item information
        Item* pItem = sAuctionMgr->GetAItem(auction->item_guid);
        if (!pItem)
        {
            if (debug_Out)
                LOG_ERROR("module", "AHBuyer: Item {} doesn't exist, perhaps bought already?", auction->item_guid.ToString());
            continue;
        }

        // get item prototype
        ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(auction->item_template);

        // check which price we have to use, startbid or if it is bidded already
        uint32 currentprice;
        if (auction->bid)
            currentprice = auction->bid;
        else
            currentprice = auction->startbid;

        // Prepare portion from maximum bid
        double bidrate = static_cast<double>(urand(1, 100)) / 100;
        long double bidMax = 0;

        // check that bid has acceptable value and take bid based on vendorprice, stacksize and quality
        if (prototype->Quality <= AHB_MAX_QUALITY)
        {
            uint32 itemPrice = getPrice(prototype, UseBuyPriceForBuyer, false) * getCustomScaling(prototype) / 125;
            if (currentprice < itemPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality))
                bidMax = itemPrice * pItem->GetCount() * config->GetBuyerPrice(prototype->Quality);
        } else
        {
            // quality is something it shouldn't be, let's get out of here
            if (debug_Out)
                LOG_ERROR("module", "AHBuyer: Quality {} not Supported", prototype->Quality);
            continue;
        }

        // check some special items, and do recalculating to their prices
        switch (prototype->Class)
        {
            // ammo
            case 6:
                bidMax = 0;
                break;
            default:
                break;
        }

        if (bidMax == 0)
        {
            // quality check failed to get bidmax, let's get out of here
            continue;
        }

        // Calculate our bid
        long double bidvalue = currentprice + ((bidMax - currentprice) * bidrate);
        // Convert to uint32
        auto bidprice = static_cast<uint32>(bidvalue);

        // Check our bid is high enough to be valid. If not, correct it to minimum.
        if ((currentprice + auction->GetAuctionOutBid()) > bidprice)
            bidprice = currentprice + auction->GetAuctionOutBid();


        if (debug_Out)
        {
            LOG_INFO("module", "-------------------------------------------------");
            LOG_INFO("module", "AHBuyer: Info for Auction #{}:", auction->Id);
            LOG_INFO("module", "AHBuyer: AuctionHouse: {}", (uint32)auction->GetHouseId());
            LOG_INFO("module", "AHBuyer: Owner: {}", auction->owner.ToString());
            LOG_INFO("module", "AHBuyer: Bidder: {}", auction->bidder.ToString());
            LOG_INFO("module", "AHBuyer: Starting Bid: {}", auction->startbid);
            LOG_INFO("module", "AHBuyer: Current Bid: {}", currentprice);
            LOG_INFO("module", "AHBuyer: Buyout: {}", auction->buyout);
            LOG_INFO("module", "AHBuyer: Deposit: {}", auction->deposit);
            LOG_INFO("module", "AHBuyer: Expire Time: {}", uint32(auction->expire_time));
            LOG_INFO("module", "AHBuyer: Bid Rate: {}", bidrate);
            LOG_INFO("module", "AHBuyer: Bid Max: {}", bidMax);
            LOG_INFO("module", "AHBuyer: Bid Value: {}", bidvalue);
            LOG_INFO("module", "AHBuyer: Bid Price: {}", bidprice);
            LOG_INFO("module", "AHBuyer: Item GUID: {}", auction->item_guid.ToString());
            LOG_INFO("module", "AHBuyer: Item Template: {}", auction->item_template);
            LOG_INFO("module", "AHBuyer: Item Info:");
            LOG_INFO("module", "AHBuyer: Item ID: {}", prototype->ItemId);
            LOG_INFO("module", "AHBuyer: Buy Price: {}", prototype->BuyPrice);
            LOG_INFO("module", "AHBuyer: Sell Price: {}", prototype->SellPrice);
            LOG_INFO("module", "AHBuyer: Bonding: {}", prototype->Bonding);
            LOG_INFO("module", "AHBuyer: Quality: {}", prototype->Quality);
            LOG_INFO("module", "AHBuyer: Item Level: {}", prototype->ItemLevel);
            LOG_INFO("module", "AHBuyer: Ammo Type: {}", prototype->AmmoType);
            LOG_INFO("module", "-------------------------------------------------");
        }

        // Check whether we do normal bid, or buyout
        if ((bidprice < auction->buyout) || (auction->buyout == 0))
        {
            if (auction->bidder)
            {
                if (auction->bidder == AHBplayer->GetGUID())
                {
                    //pl->ModifyMoney(-int32(price - auction->bid));
                } else
                {
                    // mail to last bidder and return money
                    auto trans = CharacterDatabase.BeginTransaction();
                    sAuctionMgr->SendAuctionOutbiddedMail(auction, bidprice, session->GetPlayer(), trans);
                    CharacterDatabase.CommitTransaction(trans);
                    //pl->ModifyMoney(-int32(price));
                }
            }

            auction->bidder = AHBplayer->GetGUID();
            auction->bid = bidprice;

            // Saving auction into database
            CharacterDatabase.Execute("UPDATE auctionhouse SET buyguid = '{}',lastbid = '{}' WHERE id = '{}'", auction->bidder.GetCounter(), auction->bid,auction->Id);
            sAuctionMgr->GetAuctionHouseSearcher()->UpdateBid(auction);
        } else
        {
            auto trans = CharacterDatabase.BeginTransaction();
            //buyout
            if ((auction->bidder) && (AHBplayer->GetGUID() != auction->bidder))
            {
                sAuctionMgr->SendAuctionOutbiddedMail(auction, auction->buyout, session->GetPlayer(), trans);
            }
            auction->bidder = AHBplayer->GetGUID();
            auction->bid = auction->buyout;

            // Send mails to buyer & seller
            //sAuctionMgr->SendAuctionSalePendingMail(auction, trans);
            sAuctionMgr->SendAuctionSuccessfulMail(auction, trans);
            sAuctionMgr->SendAuctionWonMail(auction, trans);
            auction->DeleteFromDB(trans);

            sAuctionMgr->RemoveAItem(auction->item_guid);
            auctionHouse->RemoveAuction(auction);
            CharacterDatabase.CommitTransaction(trans);
        }
    }
}

uint32 AuctionHouseBot::getPrice(const ItemTemplate* item, bool useBuyPrice, bool isSeller)
{
    uint32 price = 0;

    if (useBuyPrice)
    {
        price = item->BuyPrice != 0 ? item->BuyPrice : (item->SellPrice * 5);
    }
    else
    {
        price = item->SellPrice != 0 ? item->SellPrice : (item->BuyPrice / 5);
    }

    if (price == 0 && item->Class == ITEM_CLASS_CONSUMABLE && item->SubClass == ITEM_SUBCLASS_ITEM_ENHANCEMENT)
    {

        price = 500 * item->ItemLevel - 250;
    }

    if (isSeller && price == 0)
        price = 1000000;

    return price;
}

double AuctionHouseBot::getCustomScaling(const ItemTemplate* item)
{
    if (item->Class == ITEM_CLASS_GLYPH)
    {
        return 250000;
    }

    if (item->Class == ITEM_CLASS_GEM)
    {
        return 2500;
    }

    if (item->Class == ITEM_CLASS_QUEST)
    {
        return 5000;
    }

    if (item->Class == ITEM_CLASS_CONTAINER)
    {
        return 2000;
    }


    if (item->Class == ITEM_CLASS_TRADE_GOODS)
    {
        if (item->SubClass == ITEM_SUBCLASS_CLOTH)
        {
            return 100;
        }

        if (item->SubClass == ITEM_SUBCLASS_ENCHANTING)
        {
            switch (item->Quality)
            {
            case ITEM_QUALITY_UNCOMMON:
                return 25;
            case ITEM_QUALITY_RARE:
                return 80;
            }
        }
    }

    if (item->Class == ITEM_CLASS_MISC)
    {
        if (item->SubClass == ITEM_SUBCLASS_JUNK_MOUNT)
        {
            return 10000;
        }

        if (item->SubClass == ITEM_SUBCLASS_JUNK_PET)
        {
            return 7500;
        }
    }

    if (item->Class == ITEM_CLASS_WEAPON || item->Class == ITEM_CLASS_ARMOR)
    {
        if (item->RequiredLevel >= 80)
            return 3000;
        else if (item->RequiredLevel >= 71)
            return 2500;
        else if (item->RequiredLevel >= 70)
            return 2000;
        else if (item->RequiredLevel >= 61)
            return 1500;
        else if (item->RequiredLevel >= 60)
            return 1000;
    }

    return 100;
}

void AuctionHouseBot::Update()
{
    time_t _newrun = time(nullptr);
    if ((!AHBSeller) && (!AHBBuyer))
        return;

    std::string accountName = "AuctionHouseBot" + std::to_string(AHBplayerAccount);

    WorldSession _session(AHBplayerAccount, std::move(accountName), nullptr, SEC_PLAYER, sWorld->getIntConfig(CONFIG_EXPANSION), 0, LOCALE_enUS, 0, false,
                          false, 0);
    Player _AHBplayer(&_session);
    _AHBplayer.Initialize(AHBplayerGUID);
    ObjectAccessor::AddObject(&_AHBplayer);

    // Add New Bids
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        addNewAuctions(&_AHBplayer, &AllianceConfig);
        if (((_newrun - _lastrun_a) >= (AllianceConfig.GetBiddingInterval() * MINUTE)) && (AllianceConfig.GetBidsPerInterval() > 0))
        {
            //if (debug_Out) sLog->outError( "AHBuyer: %u seconds have passed since last bid", (_newrun - _lastrun_a));
            //if (debug_Out) sLog->outError( "AHBuyer: Bidding on Alliance Auctions");
            addNewAuctionBuyerBotBid(&_AHBplayer, &AllianceConfig, &_session);
            _lastrun_a = _newrun;
        }

        addNewAuctions(&_AHBplayer, &HordeConfig);
        if (((_newrun - _lastrun_h) >= (HordeConfig.GetBiddingInterval() * MINUTE)) && (HordeConfig.GetBidsPerInterval() > 0))
        {
            //if (debug_Out) sLog->outError( "AHBuyer: %u seconds have passed since last bid", (_newrun - _lastrun_h));
            //if (debug_Out) sLog->outError( "AHBuyer: Bidding on Horde Auctions");
            addNewAuctionBuyerBotBid(&_AHBplayer, &HordeConfig, &_session);
            _lastrun_h = _newrun;
        }
    }

    addNewAuctions(&_AHBplayer, &NeutralConfig);
    if (((_newrun - _lastrun_n) >= (NeutralConfig.GetBiddingInterval() * MINUTE)) && (NeutralConfig.GetBidsPerInterval() > 0))
    {
        //if (debug_Out) sLog->outError( "AHBuyer: %u seconds have passed since last bid", (_newrun - _lastrun_n));
        //if (debug_Out) sLog->outError( "AHBuyer: Bidding on Neutral Auctions");
        addNewAuctionBuyerBotBid(&_AHBplayer, &NeutralConfig, &_session);
        _lastrun_n = _newrun;
    }
    ObjectAccessor::RemoveObject(&_AHBplayer);
}

void AuctionHouseBot::Initialize()
{
    DisableItemStore.clear();
    QueryResult result = WorldDatabase.Query("SELECT item FROM mod_auctionhousebot_disabled_items");

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            DisableItemStore.insert(fields[0].Get<uint32>());
        } while (result->NextRow());
    }

    //End Filters
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION))
    {
        LoadValues(&AllianceConfig);
        LoadValues(&HordeConfig);
    }
    LoadValues(&NeutralConfig);

    //
    // check if the AHBot account/GUID in the config actually exists
    //

    if ((AHBplayerAccount != 0) || (AHBplayerGUID != 0))
    {
        QueryResult result = CharacterDatabase.Query("SELECT 1 FROM characters WHERE account = {} AND guid = {}", AHBplayerAccount, AHBplayerGUID);
        if (!result)
        {
            LOG_ERROR("module", "AuctionHouseBot: The account/GUID-information set for your AHBot is incorrect (account: {} guid: {})", AHBplayerAccount,
                      AHBplayerGUID);
            return;
        }
    }

    if (AHBSeller)
    {
        QueryResult results = QueryResult(nullptr);
        char npcQuery[] = "SELECT DISTINCT item FROM npc_vendor WHERE maxcount = 0 AND ExtendedCost = 0";
        results = WorldDatabase.Query(npcQuery);
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                npcItems.push_back(fields[0].Get<int32>());

            } while (results->NextRow());
        } else
        {
            if (debug_Out)
                LOG_ERROR("module", "AuctionHouseBot: \"{}\" failed", npcQuery);
        }

        char lootQuery[] = "SELECT item FROM creature_loot_template UNION "
                           "SELECT item FROM reference_loot_template UNION "
                           "SELECT item FROM disenchant_loot_template UNION "
                           "SELECT item FROM fishing_loot_template UNION "
                           "SELECT item FROM gameobject_loot_template UNION "
                           "SELECT item FROM item_loot_template UNION "
                           "SELECT item FROM milling_loot_template UNION "
                           "SELECT item FROM pickpocketing_loot_template UNION "
                           "SELECT item FROM prospecting_loot_template UNION "
                           "SELECT item FROM skinning_loot_template UNION " 
                           "SELECT entry FROM item_template";

        results = WorldDatabase.Query(lootQuery);
        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                lootItems.push_back(fields[0].Get<uint32>());

            } while (results->NextRow());
        } else
        {
            if (debug_Out)
                LOG_ERROR("module", "AuctionHouseBot: \"{}\" failed", lootQuery);
        }

        ItemTemplateContainer const* its = sObjectMgr->GetItemTemplateStore();
        for (auto itr = its->begin(); itr != its->end(); ++itr)
        {
            if (itr->second.Flags & ITEM_FLAG_DEPRECATED)
            {
                continue;
            }

            switch (itr->second.Bonding)
            {
                case NO_BIND:
                    if (!No_Bind)
                        continue;
                    break;
                case BIND_WHEN_PICKED_UP:
                    if (!Bind_When_Picked_Up)
                        continue;
                    break;
                case BIND_WHEN_EQUIPPED:
                    if (!Bind_When_Equipped)
                        continue;
                    break;
                case BIND_WHEN_USE:
                    if (!Bind_When_Use)
                        continue;
                    break;
                case BIND_QUEST_ITEM:
                    if (!Bind_Quest_Item)
                        continue;
                    break;
                default:
                    continue;
                    break;
            }

            if (getPrice(&itr->second) == 0)
                continue;

            if (itr->second.Quality > 6)
                continue;

            if ((Vendor_Items == 0) && itr->second.Class != ITEM_CLASS_TRADE_GOODS)
            {
                bool isVendorItem = false;

                for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorItem); i++)
                {
                    if (itr->second.ItemId == npcItems[i])
                        isVendorItem = true;
                }

                if (isVendorItem)
                    continue;
            }

            if ((Vendor_TGs == 0) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS))
            {
                bool isVendorTG = false;

                for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorTG); i++)
                {
                    if (itr->second.ItemId == npcItems[i])
                        isVendorTG = true;
                }

                if (isVendorTG)
                    continue;
            }

            if ((Loot_Items == 0) && itr->second.Class != ITEM_CLASS_TRADE_GOODS)
            {
                bool isLootItem = false;

                for (unsigned int i = 0; (i < lootItems.size()) && (!isLootItem); i++)
                {
                    if (itr->second.ItemId == lootItems[i])
                        isLootItem = true;
                }

                if (isLootItem)
                    continue;
            }

            if ((Loot_TGs == 0) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS))
            {
                bool isLootTG = false;

                for (unsigned int i = 0; (i < lootItems.size()) && (!isLootTG); i++)
                {
                    if (itr->second.ItemId == lootItems[i])
                        isLootTG = true;
                }

                if (isLootTG)
                    continue;
            }

            if ((Other_Items == 0) && itr->second.Class != ITEM_CLASS_TRADE_GOODS)
            {
                bool isVendorItem = false;
                bool isLootItem = false;

                for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorItem); i++)
                {
                    if (itr->second.ItemId == npcItems[i])
                        isVendorItem = true;
                }
                for (unsigned int i = 0; (i < lootItems.size()) && (!isLootItem); i++)
                {
                    if (itr->second.ItemId == lootItems[i])
                        isLootItem = true;
                }
                if ((!isLootItem) && (!isVendorItem))
                    continue;
            }

            if ((Other_TGs == 0) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS))
            {
                bool isVendorTG = false;
                bool isLootTG = false;

                for (unsigned int i = 0; (i < npcItems.size()) && (!isVendorTG); i++)
                {
                    if (itr->second.ItemId == npcItems[i])
                        isVendorTG = true;
                }
                for (unsigned int i = 0; (i < lootItems.size()) && (!isLootTG); i++)
                {
                    if (itr->second.ItemId == lootItems[i])
                        isLootTG = true;
                }
                if ((!isLootTG) && (!isVendorTG))
                {
                    LOG_ERROR("module", "AuctionHouseBot: Item {} IS OTHER_TG", itr->second.ItemId);
                    continue;
                }
            }

            // Disable items by Id
            if (DisableItemStore.find(itr->second.ItemId) != DisableItemStore.end())
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (PTR/Beta/Unused Item)", itr->second.ItemId);
                continue;
            }

            // Disable permanent enchants items
            if ((DisablePermEnchant) && (itr->second.Class == ITEM_CLASS_PERMANENT))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Permanent Enchant Item)", itr->second.ItemId);
                continue;
            }

            // Disable conjured items
            if ((DisableConjured) && (itr->second.IsConjuredConsumable()))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Conjured Consumable)", itr->second.ItemId);
                continue;
            }

            // Disable gems
            if ((DisableGems) && (itr->second.Class == ITEM_CLASS_GEM))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Gem)", itr->second.ItemId);
                continue;
            }

            // Disable money
            if ((DisableMoney) && (itr->second.Class == ITEM_CLASS_MONEY))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Money)", itr->second.ItemId);
                continue;
            }

            // Disable moneyloot
            if ((DisableMoneyLoot) && (itr->second.MinMoneyLoot > 0))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (MoneyLoot)", itr->second.ItemId);
                continue;
            }

            // Disable lootable items
            if ((DisableLootable) && (itr->second.Flags & 4))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Lootable Item)", itr->second.ItemId);
                continue;
            }

            // Disable Keys
            if ((DisableKeys) && (itr->second.Class == ITEM_CLASS_KEY))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Quest Item)", itr->second.ItemId);
                continue;
            }

            // Disable items with duration
            if ((DisableDuration) && (itr->second.Duration > 0))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Has a Duration)", itr->second.ItemId);
                continue;
            }

            // Disable items which are BOP or Quest Items and have a required level lower than the item level
            if ((DisableBOP_Or_Quest_NoReqLevel) &&
                ((itr->second.Bonding == BIND_WHEN_PICKED_UP || itr->second.Bonding == BIND_QUEST_ITEM) && (itr->second.RequiredLevel < itr->second.ItemLevel)))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (BOP or BQI and Required Level is less than Item Level)", itr->second.ItemId);
                continue;
            }

            // Disable items that are pets
            if (DisablePets && itr->second.Class == ITEM_CLASS_MISC && itr->second.SubClass == ITEM_SUBCLASS_JUNK_PET)
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Pet)", itr->second.ItemId);
                continue;
            }

            // Disable items that are mounts
            if (DisableMounts && itr->second.Class == ITEM_CLASS_MISC && itr->second.SubClass == ITEM_SUBCLASS_JUNK_MOUNT)
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Mount)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Warrior
            if ((DisableWarriorItems) && (itr->second.AllowableClass == 1))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Warrior Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Paladin
            if ((DisablePaladinItems) && (itr->second.AllowableClass == 2))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Paladin Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Hunter
            if ((DisableHunterItems) && (itr->second.AllowableClass == 4))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Hunter Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Rogue
            if ((DisableRogueItems) && (itr->second.AllowableClass == 8))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Rogue Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Priest
            if ((DisablePriestItems) && (itr->second.AllowableClass == 16))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Priest Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for DK
            if ((DisableDKItems) && (itr->second.AllowableClass == 32))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (DK Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Shaman
            if ((DisableShamanItems) && (itr->second.AllowableClass == 64))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Shaman Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Mage
            if ((DisableMageItems) && (itr->second.AllowableClass == 128))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Mage Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Warlock
            if ((DisableWarlockItems) && (itr->second.AllowableClass == 256))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Warlock Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Unused Class
            if ((DisableUnusedClassItems) && (itr->second.AllowableClass == 512))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Unused Item)", itr->second.ItemId);
                continue;
            }

            // Disable items specifically for Druid
            if ((DisableDruidItems) && (itr->second.AllowableClass == 1024))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Druid Item)", itr->second.ItemId);
                continue;
            }

            // Disable Items below level X
            if ((DisableItemsBelowLevel) && (itr->second.Class != ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemLevel < DisableItemsBelowLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Item Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Items above level X
            if ((DisableItemsAboveLevel) && (itr->second.Class != ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemLevel > DisableItemsAboveLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Item Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Trade Goods below level X
            if ((DisableTGsBelowLevel) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemLevel < DisableTGsBelowLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Trade Good {} disabled (Trade Good Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Trade Goods above level X
            if ((DisableTGsAboveLevel) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemLevel > DisableTGsAboveLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Trade Good {} disabled (Trade Good Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Items below GUID X
            if ((DisableItemsBelowGUID) && (itr->second.Class != ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemId < DisableItemsBelowGUID))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Item Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Items above GUID X
            if ((DisableItemsAboveGUID) && (itr->second.Class != ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemId > DisableItemsAboveGUID))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Item Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Trade Goods below GUID X
            if ((DisableTGsBelowGUID) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemId < DisableTGsBelowGUID))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Trade Good Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Trade Goods above GUID X
            if ((DisableTGsAboveGUID) && (itr->second.Class == ITEM_CLASS_TRADE_GOODS) && (itr->second.ItemId > DisableTGsAboveGUID))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (Trade Good Level = {})", itr->second.ItemId, itr->second.ItemLevel);
                continue;
            }

            // Disable Items for level lower than X
            if ((DisableItemsBelowReqLevel) && (itr->second.RequiredLevel < DisableItemsBelowReqLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (RequiredLevel = {})", itr->second.ItemId, itr->second.RequiredLevel);
                continue;
            }

            // Disable Items for level higher than X
            if ((DisableItemsAboveReqLevel) && (itr->second.RequiredLevel > DisableItemsAboveReqLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (RequiredLevel = {})", itr->second.ItemId, itr->second.RequiredLevel);
                continue;
            }

            // Disable Trade Goods for level lower than X
            if ((DisableTGsBelowReqLevel) && (itr->second.RequiredLevel < DisableTGsBelowReqLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Trade Good {} disabled (RequiredLevel = {})", itr->second.ItemId, itr->second.RequiredLevel);
                continue;
            }

            // Disable Trade Goods for level higher than X
            if ((DisableTGsAboveReqLevel) && (itr->second.RequiredLevel > DisableTGsAboveReqLevel))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Trade Good {} disabled (RequiredLevel = {})", itr->second.ItemId, itr->second.RequiredLevel);
                continue;
            }

            // Disable Items that require skill lower than X
            if ((DisableItemsBelowReqSkillRank) && (itr->second.RequiredSkillRank < DisableItemsBelowReqSkillRank))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (RequiredSkillRank = {})", itr->second.ItemId, itr->second.RequiredSkillRank);
                continue;
            }

            // Disable Items that require skill higher than X
            if ((DisableItemsAboveReqSkillRank) && (itr->second.RequiredSkillRank > DisableItemsAboveReqSkillRank))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (RequiredSkillRank = {})", itr->second.ItemId, itr->second.RequiredSkillRank);
                continue;
            }

            // Disable Trade Goods that require skill lower than X
            if ((DisableTGsBelowReqSkillRank) && (itr->second.RequiredSkillRank < DisableTGsBelowReqSkillRank))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (RequiredSkillRank = {})", itr->second.ItemId, itr->second.RequiredSkillRank);
                continue;
            }

            // Disable Trade Goods that require skill higher than X
            if ((DisableTGsAboveReqSkillRank) && (itr->second.RequiredSkillRank > DisableTGsAboveReqSkillRank))
            {
                if (debug_Out_Filters)
                    LOG_ERROR("module", "AuctionHouseBot: Item {} disabled (RequiredSkillRank = {})", itr->second.ItemId, itr->second.RequiredSkillRank);
                continue;
            }

            switch (itr->second.Quality)
            {
                case AHB_GREY:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        greyTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        greyItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_WHITE:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        whiteTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        whiteItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_GREEN:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        greenTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        greenItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_BLUE:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        blueTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        blueItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_PURPLE:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        purpleTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        purpleItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_ORANGE:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        orangeTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        orangeItemsBin.push_back(itr->second.ItemId);
                    break;

                case AHB_YELLOW:
                    if (itr->second.Class == ITEM_CLASS_TRADE_GOODS)
                        yellowTradeGoodsBin.push_back(itr->second.ItemId);
                    else
                        yellowItemsBin.push_back(itr->second.ItemId);
                    break;
            }
        }

        if ((greyTradeGoodsBin.empty()) &&
            (whiteTradeGoodsBin.empty()) &&
            (greenTradeGoodsBin.empty()) &&
            (blueTradeGoodsBin.empty()) &&
            (purpleTradeGoodsBin.empty()) &&
            (orangeTradeGoodsBin.empty()) &&
            (yellowTradeGoodsBin.empty()) &&
            (greyItemsBin.empty()) &&
            (whiteItemsBin.empty()) &&
            (greenItemsBin.empty()) &&
            (blueItemsBin.empty()) &&
            (purpleItemsBin.empty()) &&
            (orangeItemsBin.empty()) &&
            (yellowItemsBin.empty()))
        {
            LOG_ERROR("module", "AuctionHouseBot: No items");
            AHBSeller = false;
        }

        LOG_INFO("module", "AuctionHouseBot:");
        LOG_INFO("module", "{} disabled items", uint32(DisableItemStore.size()));
        LOG_INFO("module", "loaded {} grey trade goods", uint32(greyTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} white trade goods", uint32(whiteTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} green trade goods", uint32(greenTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} blue trade goods", uint32(blueTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} purple trade goods", uint32(purpleTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} orange trade goods", uint32(orangeTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} yellow trade goods", uint32(yellowTradeGoodsBin.size()));
        LOG_INFO("module", "loaded {} grey items", uint32(greyItemsBin.size()));
        LOG_INFO("module", "loaded {} white items", uint32(whiteItemsBin.size()));
        LOG_INFO("module", "loaded {} green items", uint32(greenItemsBin.size()));
        LOG_INFO("module", "loaded {} blue items", uint32(blueItemsBin.size()));
        LOG_INFO("module", "loaded {} purple items", uint32(purpleItemsBin.size()));
        LOG_INFO("module", "loaded {} orange items", uint32(orangeItemsBin.size()));
        LOG_INFO("module", "loaded {} yellow items", uint32(yellowItemsBin.size()));
    }

    LOG_INFO("module", "AuctionHouseBot and AuctionHouseBuyer have been loaded.");
}

void AuctionHouseBot::InitializeConfiguration()
{
    debug_Out = sConfigMgr->GetOption<bool>("AuctionHouseBot.DEBUG", false);
    debug_Out_Filters = sConfigMgr->GetOption<bool>("AuctionHouseBot.DEBUG_FILTERS", false);

    AHBSeller = sConfigMgr->GetOption<bool>("AuctionHouseBot.EnableSeller", false);
    AHBBuyer = sConfigMgr->GetOption<bool>("AuctionHouseBot.EnableBuyer", false);
    UseBuyPriceForSeller = sConfigMgr->GetOption<bool>("AuctionHouseBot.UseBuyPriceForSeller", false);
    UseBuyPriceForBuyer = sConfigMgr->GetOption<bool>("AuctionHouseBot.UseBuyPriceForBuyer", false);

    AHBplayerAccount = sConfigMgr->GetOption<uint32>("AuctionHouseBot.Account", 0);
    AHBplayerGUID = sConfigMgr->GetOption<uint32>("AuctionHouseBot.GUID", 0);
    ItemsPerCycle = sConfigMgr->GetOption<uint32>("AuctionHouseBot.ItemsPerCycle", 200);

    //Begin Filters

    Vendor_Items = sConfigMgr->GetOption<bool>("AuctionHouseBot.VendorItems", false);
    Loot_Items = sConfigMgr->GetOption<bool>("AuctionHouseBot.LootItems", true);
    Other_Items = sConfigMgr->GetOption<bool>("AuctionHouseBot.OtherItems", false);
    Vendor_TGs = sConfigMgr->GetOption<bool>("AuctionHouseBot.VendorTradeGoods", false);
    Loot_TGs = sConfigMgr->GetOption<bool>("AuctionHouseBot.LootTradeGoods", true);
    Other_TGs = sConfigMgr->GetOption<bool>("AuctionHouseBot.OtherTradeGoods", false);

    No_Bind = sConfigMgr->GetOption<bool>("AuctionHouseBot.No_Bind", true);
    Bind_When_Picked_Up = sConfigMgr->GetOption<bool>("AuctionHouseBot.Bind_When_Picked_Up", false);
    Bind_When_Equipped = sConfigMgr->GetOption<bool>("AuctionHouseBot.Bind_When_Equipped", true);
    Bind_When_Use = sConfigMgr->GetOption<bool>("AuctionHouseBot.Bind_When_Use", true);
    Bind_Quest_Item = sConfigMgr->GetOption<bool>("AuctionHouseBot.Bind_Quest_Item", false);

    DisablePermEnchant = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisablePermEnchant", false);
    DisableConjured = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableConjured", false);
    DisableGems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableGems", false);
    DisableMoney = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableMoney", false);
    DisableMoneyLoot = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableMoneyLoot", false);
    DisableLootable = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableLootable", false);
    DisableKeys = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableKeys", false);
    DisableDuration = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableDuration", false);
    DisableBOP_Or_Quest_NoReqLevel = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableBOP_Or_Quest_NoReqLevel", false);
    DisablePets = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisablePets", false);
    DisableMounts = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableMounts", false);

    DisableWarriorItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableWarriorItems", false);
    DisablePaladinItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisablePaladinItems", false);
    DisableHunterItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableHunterItems", false);
    DisableRogueItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableRogueItems", false);
    DisablePriestItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisablePriestItems", false);
    DisableDKItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableDKItems", false);
    DisableShamanItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableShamanItems", false);
    DisableMageItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableMageItems", false);
    DisableWarlockItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableWarlockItems", false);
    DisableUnusedClassItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableUnusedClassItems", false);
    DisableDruidItems = sConfigMgr->GetOption<bool>("AuctionHouseBot.DisableDruidItems", false);

    DisableItemsBelowLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsBelowLevel", 0);
    DisableItemsAboveLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsAboveLevel", 0);
    DisableTGsBelowLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsBelowLevel", 0);
    DisableTGsAboveLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsAboveLevel", 0);
    DisableItemsBelowGUID = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsBelowGUID", 0);
    DisableItemsAboveGUID = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsAboveGUID", 0);
    DisableTGsBelowGUID = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsBelowGUID", 0);
    DisableTGsAboveGUID = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsAboveGUID", 0);
    DisableItemsBelowReqLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsBelowReqLevel", 0);
    DisableItemsAboveReqLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsAboveReqLevel", 0);
    DisableTGsBelowReqLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsBelowReqLevel", 0);
    DisableTGsAboveReqLevel = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsAboveReqLevel", 0);
    DisableItemsBelowReqSkillRank = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsBelowReqSkillRank", 0);
    DisableItemsAboveReqSkillRank = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableItemsAboveReqSkillRank", 0);
    DisableTGsBelowReqSkillRank = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsBelowReqSkillRank", 0);
    DisableTGsAboveReqSkillRank = sConfigMgr->GetOption<uint32>("AuctionHouseBot.DisableTGsAboveReqSkillRank", 0);
}

void AuctionHouseBot::IncrementItemCounts(AuctionEntry* ah)
{
    // from auctionhousehandler.cpp, creates auction pointer & player pointer

    // get exact item information
    Item* pItem = sAuctionMgr->GetAItem(ah->item_guid);
    if (!pItem)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: Item {} doesn't exist, perhaps bought already?", ah->item_guid.ToString());
        return;
    }

    // get item prototype
    ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(ah->item_template);

    AHBConfig* config;

    AuctionHouseEntry const* ahEntry = sAuctionMgr->GetAuctionHouseEntryFromHouse(ah->GetHouseId());
    if (!ahEntry)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Neutral", ah->GetHouseId());
        config = &NeutralConfig;
    } else if (AuctionHouseId(ahEntry->houseId) == AuctionHouseId::Alliance)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Alliance", ah->GetHouseId());
        config = &AllianceConfig;
    } else if (AuctionHouseId(ahEntry->houseId) == AuctionHouseId::Horde)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Horde", ah->GetHouseId());
        config = &HordeConfig;
    } else
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Neutral", ah->GetHouseId());
        config = &NeutralConfig;
    }

    config->IncItemCounts(prototype->Class, prototype->Quality);
}

void AuctionHouseBot::DecrementItemCounts(AuctionEntry* ah, uint32 itemEntry)
{
    // get item prototype
    ItemTemplate const* prototype = sObjectMgr->GetItemTemplate(itemEntry);

    AHBConfig* config;

    AuctionHouseEntry const* ahEntry = sAuctionMgr->GetAuctionHouseEntryFromHouse(ah->GetHouseId());
    if (!ahEntry)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Neutral", ah->GetHouseId());
        config = &NeutralConfig;
    } else if (AuctionHouseId(ahEntry->houseId) == AuctionHouseId::Alliance)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Alliance", ah->GetHouseId());
        config = &AllianceConfig;
    } else if (AuctionHouseId(ahEntry->houseId) == AuctionHouseId::Horde)
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Horde", ah->GetHouseId());
        config = &HordeConfig;
    } else
    {
        if (debug_Out)
            LOG_ERROR("module", "AHBot: {} returned as House Faction. Neutral", ah->GetHouseId());
        config = &NeutralConfig;
    }

    config->DecItemCounts(prototype->Class, prototype->Quality);
}

void AuctionHouseBot::Commands(uint32 command, uint32 ahMapID, uint32 col, char* args)
{
    AHBConfig* config = nullptr;
    switch (ahMapID)
    {
        case 2:
            config = &AllianceConfig;
            break;
        case 6:
            config = &HordeConfig;
            break;
        case 7:
            config = &NeutralConfig;
            break;
    }
    std::string color;
    switch (col)
    {
        case AHB_GREY:
            color = "grey";
            break;
        case AHB_WHITE:
            color = "white";
            break;
        case AHB_GREEN:
            color = "green";
            break;
        case AHB_BLUE:
            color = "blue";
            break;
        case AHB_PURPLE:
            color = "purple";
            break;
        case AHB_ORANGE:
            color = "orange";
            break;
        case AHB_YELLOW:
            color = "yellow";
            break;
        default:
            break;
    }
    switch (command)
    {
        case 0:     //ahexpire
        {
            AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());

            AuctionHouseObject::AuctionEntryMap::iterator itr;
            itr = auctionHouse->GetAuctionsBegin();

            while (itr != auctionHouse->GetAuctionsEnd())
            {
                if (itr->second->owner.GetCounter() == AHBplayerGUID)
                {
                    itr->second->expire_time = GameTime::GetGameTime().count();
                    uint32 id = itr->second->Id;
                    uint32 expire_time = itr->second->expire_time;
                    CharacterDatabase.Execute("UPDATE auctionhouse SET time = '{}' WHERE id = '{}'", expire_time, id);
                }
                ++itr;
            }
        }
            break;
        case 1:     //min items
        {
            char* param1 = strtok(args, " ");
            auto minItems = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minitems = '{}' WHERE auctionhouse = '{}'", minItems, ahMapID);
            config->SetMinItems(minItems);
        }
            break;
        case 2:     //max items
        {
            char* param1 = strtok(args, " ");
            auto maxItems = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxitems = '{}' WHERE auctionhouse = '{}'", maxItems, ahMapID);
            config->SetMaxItems(maxItems);
            config->CalculatePercents();
        }
            break;
        case 3:     //mintime Deprecated (Place holder for future commands)
        case 4:     //max time Deprecated (Place holder for future commands)
            break;
        case 5:     //percentages
        {
            char* param1 = strtok(args, " ");
            char* param2 = strtok(nullptr, " ");
            char* param3 = strtok(nullptr, " ");
            char* param4 = strtok(nullptr, " ");
            char* param5 = strtok(nullptr, " ");
            char* param6 = strtok(nullptr, " ");
            char* param7 = strtok(nullptr, " ");
            char* param8 = strtok(nullptr, " ");
            char* param9 = strtok(nullptr, " ");
            char* param10 = strtok(nullptr, " ");
            char* param11 = strtok(nullptr, " ");
            char* param12 = strtok(nullptr, " ");
            char* param13 = strtok(nullptr, " ");
            char* param14 = strtok(nullptr, " ");
            auto greytg = (uint32) strtoul(param1, nullptr, 0);
            auto whitetg = (uint32) strtoul(param2, nullptr, 0);
            auto greentg = (uint32) strtoul(param3, nullptr, 0);
            auto bluetg = (uint32) strtoul(param4, nullptr, 0);
            auto purpletg = (uint32) strtoul(param5, nullptr, 0);
            auto orangetg = (uint32) strtoul(param6, nullptr, 0);
            auto yellowtg = (uint32) strtoul(param7, nullptr, 0);
            auto greyi = (uint32) strtoul(param8, nullptr, 0);
            auto whitei = (uint32) strtoul(param9, nullptr, 0);
            auto greeni = (uint32) strtoul(param10, nullptr, 0);
            auto bluei = (uint32) strtoul(param11, nullptr, 0);
            auto purplei = (uint32) strtoul(param12, nullptr, 0);
            auto orangei = (uint32) strtoul(param13, nullptr, 0);
            auto yellowi = (uint32) strtoul(param14, nullptr, 0);

            auto trans = WorldDatabase.BeginTransaction();
            trans->Append("UPDATE mod_auctionhousebot SET percentgreytradegoods = '{}' WHERE auctionhouse = '{}'", greytg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentwhitetradegoods = '{}' WHERE auctionhouse = '{}'", whitetg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentgreentradegoods = '{}' WHERE auctionhouse = '{}'", greentg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentbluetradegoods = '{}' WHERE auctionhouse = '{}'", bluetg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentpurpletradegoods = '{}' WHERE auctionhouse = '{}'", purpletg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentorangetradegoods = '{}' WHERE auctionhouse = '{}'", orangetg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentyellowtradegoods = '{}' WHERE auctionhouse = '{}'", yellowtg, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentgreyitems = '{}' WHERE auctionhouse = '{}'", greyi, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentwhiteitems = '{}' WHERE auctionhouse = '{}'", whitei, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentgreenitems = '{}' WHERE auctionhouse = '{}'", greeni, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentblueitems = '{}' WHERE auctionhouse = '{}'", bluei, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentpurpleitems = '{}' WHERE auctionhouse = '{}'", purplei, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentorangeitems = '{}' WHERE auctionhouse = '{}'", orangei, ahMapID);
            trans->Append("UPDATE mod_auctionhousebot SET percentyellowitems = '{}' WHERE auctionhouse = '{}'", yellowi, ahMapID);
            WorldDatabase.CommitTransaction(trans);
            config->SetPercentages(greytg, whitetg, greentg, bluetg, purpletg, orangetg, yellowtg, greyi, whitei, greeni, bluei, purplei, orangei, yellowi);
        }
            break;
        case 6:     //min prices
        {
            char* param1 = strtok(args, " ");
            auto minPrice = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minprice{} = '{}' WHERE auctionhouse = '{}'", color, minPrice, ahMapID);
            config->SetMinPrice(col, minPrice);
        }
            break;
        case 7:     //max prices
        {
            char* param1 = strtok(args, " ");
            auto maxPrice = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxprice{} = '{}' WHERE auctionhouse = '{}'", color, maxPrice, ahMapID);
            config->SetMaxPrice(col, maxPrice);
        }
            break;
        case 8:     //min bid price
        {
            char* param1 = strtok(args, " ");
            auto minBidPrice = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET minbidprice{} = '{}' WHERE auctionhouse = '{}'", color, minBidPrice, ahMapID);
            config->SetMinBidPrice(col, minBidPrice);
        }
            break;
        case 9:     //max bid price
        {
            char* param1 = strtok(args, " ");
            auto maxBidPrice = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxbidprice{} = '{}' WHERE auctionhouse = '{}'", color, maxBidPrice, ahMapID);
            config->SetMaxBidPrice(col, maxBidPrice);
        }
            break;
        case 10:        //max stacks
        {
            char* param1 = strtok(args, " ");
            auto maxStack = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET maxstack{} = '{}' WHERE auctionhouse = '{}'", color, maxStack, ahMapID);
            config->SetMaxStack(col, maxStack);
        }
            break;
        case 11:        //buyer bid prices
        {
            char* param1 = strtok(args, " ");
            auto buyerPrice = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerprice{} = '{}' WHERE auctionhouse = '{}'", color, buyerPrice, ahMapID);
            config->SetBuyerPrice(col, buyerPrice);
        }
            break;
        case 12:        //buyer bidding interval
        {
            char* param1 = strtok(args, " ");
            auto bidInterval = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerbiddinginterval = '{}' WHERE auctionhouse = '{}'", bidInterval, ahMapID);
            config->SetBiddingInterval(bidInterval);
        }
            break;
        case 13:        //buyer bids per interval
        {
            char* param1 = strtok(args, " ");
            auto bidsPerInterval = (uint32) strtoul(param1, nullptr, 0);
            WorldDatabase.Execute("UPDATE mod_auctionhousebot SET buyerbidsperinterval = '{}' WHERE auctionhouse = '{}'", bidsPerInterval, ahMapID);
            config->SetBidsPerInterval(bidsPerInterval);
        }
            break;
        default:
            break;
    }
}

void AuctionHouseBot::LoadValues(AHBConfig* config)
{
    if (debug_Out)
        LOG_ERROR("module", "Start Settings for {} Auctionhouses:",
                  WorldDatabase.Query("SELECT name FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<std::string_view>());

    if (AHBSeller)
    {
        //load min and max items
        config->SetMinItems(WorldDatabase.Query("SELECT minitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxItems(WorldDatabase.Query("SELECT maxitems FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        //load percentages
        uint32 greytg = WorldDatabase.Query("SELECT percentgreytradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                            config->GetAHID())->Fetch()->Get<uint32>();
        uint32 whitetg = WorldDatabase.Query("SELECT percentwhitetradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                             config->GetAHID())->Fetch()->Get<uint32>();
        uint32 greentg = WorldDatabase.Query("SELECT percentgreentradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                             config->GetAHID())->Fetch()->Get<uint32>();
        uint32 bluetg = WorldDatabase.Query("SELECT percentbluetradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                            config->GetAHID())->Fetch()->Get<uint32>();
        uint32 purpletg = WorldDatabase.Query("SELECT percentpurpletradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                              config->GetAHID())->Fetch()->Get<uint32>();
        uint32 orangetg = WorldDatabase.Query("SELECT percentorangetradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                              config->GetAHID())->Fetch()->Get<uint32>();
        uint32 yellowtg = WorldDatabase.Query("SELECT percentyellowtradegoods FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                              config->GetAHID())->Fetch()->Get<uint32>();
        uint32 greyi = WorldDatabase.Query("SELECT percentgreyitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                           config->GetAHID())->Fetch()->Get<uint32>();
        uint32 whitei = WorldDatabase.Query("SELECT percentwhiteitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                            config->GetAHID())->Fetch()->Get<uint32>();
        uint32 greeni = WorldDatabase.Query("SELECT percentgreenitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                            config->GetAHID())->Fetch()->Get<uint32>();
        uint32 bluei = WorldDatabase.Query("SELECT percentblueitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                           config->GetAHID())->Fetch()->Get<uint32>();
        uint32 purplei = WorldDatabase.Query("SELECT percentpurpleitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                             config->GetAHID())->Fetch()->Get<uint32>();
        uint32 orangei = WorldDatabase.Query("SELECT percentorangeitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                             config->GetAHID())->Fetch()->Get<uint32>();
        uint32 yellowi = WorldDatabase.Query("SELECT percentyellowitems FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                             config->GetAHID())->Fetch()->Get<uint32>();
        config->SetPercentages(greytg, whitetg, greentg, bluetg, purpletg, orangetg, yellowtg, greyi, whitei, greeni, bluei, purplei, orangei, yellowi);
        //load min and max prices
        config->SetMinPrice(AHB_GREY, WorldDatabase.Query("SELECT minpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                          config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_GREY, WorldDatabase.Query("SELECT maxpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                          config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinPrice(AHB_WHITE, WorldDatabase.Query("SELECT minpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                           config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_WHITE, WorldDatabase.Query("SELECT maxpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                           config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinPrice(AHB_GREEN, WorldDatabase.Query("SELECT minpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                           config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_GREEN, WorldDatabase.Query("SELECT maxpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                           config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinPrice(AHB_BLUE, WorldDatabase.Query("SELECT minpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                          config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_BLUE, WorldDatabase.Query("SELECT maxpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                          config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinPrice(AHB_PURPLE, WorldDatabase.Query("SELECT minpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_PURPLE, WorldDatabase.Query("SELECT maxpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinPrice(AHB_ORANGE, WorldDatabase.Query("SELECT minpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_ORANGE, WorldDatabase.Query("SELECT maxpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinPrice(AHB_YELLOW, WorldDatabase.Query("SELECT minpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxPrice(AHB_YELLOW, WorldDatabase.Query("SELECT maxpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        //load min and max bid prices
        config->SetMinBidPrice(AHB_GREY, WorldDatabase.Query("SELECT minbidpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                             config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_GREY, WorldDatabase.Query("SELECT maxbidpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                             config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinBidPrice(AHB_WHITE, WorldDatabase.Query("SELECT minbidpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_WHITE, WorldDatabase.Query("SELECT maxbidpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinBidPrice(AHB_GREEN, WorldDatabase.Query("SELECT minbidpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_GREEN, WorldDatabase.Query("SELECT maxbidpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinBidPrice(AHB_BLUE, WorldDatabase.Query("SELECT minbidpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                             config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_BLUE, WorldDatabase.Query("SELECT maxbidpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                             config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinBidPrice(AHB_PURPLE, WorldDatabase.Query("SELECT minbidpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                               config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_PURPLE, WorldDatabase.Query("SELECT maxbidpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                               config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinBidPrice(AHB_ORANGE, WorldDatabase.Query("SELECT minbidpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                               config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_ORANGE, WorldDatabase.Query("SELECT maxbidpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                               config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMinBidPrice(AHB_YELLOW, WorldDatabase.Query("SELECT minbidpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                               config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxBidPrice(AHB_YELLOW, WorldDatabase.Query("SELECT maxbidpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                               config->GetAHID())->Fetch()->Get<uint32>());
        //load max stacks
        config->SetMaxStack(AHB_GREY, WorldDatabase.Query("SELECT maxstackgrey FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                          config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxStack(AHB_WHITE, WorldDatabase.Query("SELECT maxstackwhite FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                           config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxStack(AHB_GREEN, WorldDatabase.Query("SELECT maxstackgreen FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                           config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxStack(AHB_BLUE, WorldDatabase.Query("SELECT maxstackblue FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                          config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxStack(AHB_PURPLE, WorldDatabase.Query("SELECT maxstackpurple FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxStack(AHB_ORANGE, WorldDatabase.Query("SELECT maxstackorange FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetMaxStack(AHB_YELLOW, WorldDatabase.Query("SELECT maxstackyellow FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        if (debug_Out)
        {
            LOG_ERROR("module", "minItems                = {}", config->GetMinItems());
            LOG_ERROR("module", "maxItems                = {}", config->GetMaxItems());
            LOG_ERROR("module", "percentGreyTradeGoods   = {}", config->GetPercentages(AHB_GREY_TG));
            LOG_ERROR("module", "percentWhiteTradeGoods  = {}", config->GetPercentages(AHB_WHITE_TG));
            LOG_ERROR("module", "percentGreenTradeGoods  = {}", config->GetPercentages(AHB_GREEN_TG));
            LOG_ERROR("module", "percentBlueTradeGoods   = {}", config->GetPercentages(AHB_BLUE_TG));
            LOG_ERROR("module", "percentPurpleTradeGoods = {}", config->GetPercentages(AHB_PURPLE_TG));
            LOG_ERROR("module", "percentOrangeTradeGoods = {}", config->GetPercentages(AHB_ORANGE_TG));
            LOG_ERROR("module", "percentYellowTradeGoods = {}", config->GetPercentages(AHB_YELLOW_TG));
            LOG_ERROR("module", "percentGreyItems        = {}", config->GetPercentages(AHB_GREY_I));
            LOG_ERROR("module", "percentWhiteItems       = {}", config->GetPercentages(AHB_WHITE_I));
            LOG_ERROR("module", "percentGreenItems       = {}", config->GetPercentages(AHB_GREEN_I));
            LOG_ERROR("module", "percentBlueItems        = {}", config->GetPercentages(AHB_BLUE_I));
            LOG_ERROR("module", "percentPurpleItems      = {}", config->GetPercentages(AHB_PURPLE_I));
            LOG_ERROR("module", "percentOrangeItems      = {}", config->GetPercentages(AHB_ORANGE_I));
            LOG_ERROR("module", "percentYellowItems      = {}", config->GetPercentages(AHB_YELLOW_I));
            LOG_ERROR("module", "minPriceGrey            = {}", config->GetMinPrice(AHB_GREY));
            LOG_ERROR("module", "maxPriceGrey            = {}", config->GetMaxPrice(AHB_GREY));
            LOG_ERROR("module", "minPriceWhite           = {}", config->GetMinPrice(AHB_WHITE));
            LOG_ERROR("module", "maxPriceWhite           = {}", config->GetMaxPrice(AHB_WHITE));
            LOG_ERROR("module", "minPriceGreen           = {}", config->GetMinPrice(AHB_GREEN));
            LOG_ERROR("module", "maxPriceGreen           = {}", config->GetMaxPrice(AHB_GREEN));
            LOG_ERROR("module", "minPriceBlue            = {}", config->GetMinPrice(AHB_BLUE));
            LOG_ERROR("module", "maxPriceBlue            = {}", config->GetMaxPrice(AHB_BLUE));
            LOG_ERROR("module", "minPricePurple          = {}", config->GetMinPrice(AHB_PURPLE));
            LOG_ERROR("module", "maxPricePurple          = {}", config->GetMaxPrice(AHB_PURPLE));
            LOG_ERROR("module", "minPriceOrange          = {}", config->GetMinPrice(AHB_ORANGE));
            LOG_ERROR("module", "maxPriceOrange          = {}", config->GetMaxPrice(AHB_ORANGE));
            LOG_ERROR("module", "minPriceYellow          = {}", config->GetMinPrice(AHB_YELLOW));
            LOG_ERROR("module", "maxPriceYellow          = {}", config->GetMaxPrice(AHB_YELLOW));
            LOG_ERROR("module", "minBidPriceGrey         = {}", config->GetMinBidPrice(AHB_GREY));
            LOG_ERROR("module", "maxBidPriceGrey         = {}", config->GetMaxBidPrice(AHB_GREY));
            LOG_ERROR("module", "minBidPriceWhite        = {}", config->GetMinBidPrice(AHB_WHITE));
            LOG_ERROR("module", "maxBidPriceWhite        = {}", config->GetMaxBidPrice(AHB_WHITE));
            LOG_ERROR("module", "minBidPriceGreen        = {}", config->GetMinBidPrice(AHB_GREEN));
            LOG_ERROR("module", "maxBidPriceGreen        = {}", config->GetMaxBidPrice(AHB_GREEN));
            LOG_ERROR("module", "minBidPriceBlue         = {}", config->GetMinBidPrice(AHB_BLUE));
            LOG_ERROR("module", "maxBidPriceBlue         = {}", config->GetMinBidPrice(AHB_BLUE));
            LOG_ERROR("module", "minBidPricePurple       = {}", config->GetMinBidPrice(AHB_PURPLE));
            LOG_ERROR("module", "maxBidPricePurple       = {}", config->GetMaxBidPrice(AHB_PURPLE));
            LOG_ERROR("module", "minBidPriceOrange       = {}", config->GetMinBidPrice(AHB_ORANGE));
            LOG_ERROR("module", "maxBidPriceOrange       = {}", config->GetMaxBidPrice(AHB_ORANGE));
            LOG_ERROR("module", "minBidPriceYellow       = {}", config->GetMinBidPrice(AHB_YELLOW));
            LOG_ERROR("module", "maxBidPriceYellow       = {}", config->GetMaxBidPrice(AHB_YELLOW));
            LOG_ERROR("module", "maxStackGrey            = {}", config->GetMaxStack(AHB_GREY));
            LOG_ERROR("module", "maxStackWhite           = {}", config->GetMaxStack(AHB_WHITE));
            LOG_ERROR("module", "maxStackGreen           = {}", config->GetMaxStack(AHB_GREEN));
            LOG_ERROR("module", "maxStackBlue            = {}", config->GetMaxStack(AHB_BLUE));
            LOG_ERROR("module", "maxStackPurple          = {}", config->GetMaxStack(AHB_PURPLE));
            LOG_ERROR("module", "maxStackOrange          = {}", config->GetMaxStack(AHB_ORANGE));
            LOG_ERROR("module", "maxStackYellow          = {}", config->GetMaxStack(AHB_YELLOW));
        }

        //AuctionHouseEntry const* ahEntry =  AuctionHouseMgr::GetAuctionHouseEntry(config->GetAHFID());
        AuctionHouseObject* auctionHouse = sAuctionMgr->GetAuctionsMap(config->GetAHFID());

        config->ResetItemCounts();
        uint32 auctions = auctionHouse->Getcount();

        if (auctions)
        {
            for (AuctionHouseObject::AuctionEntryMap::const_iterator itr = auctionHouse->GetAuctionsBegin(); itr != auctionHouse->GetAuctionsEnd(); ++itr)
            {
                AuctionEntry* Aentry = itr->second;
                Item* item = sAuctionMgr->GetAItem(Aentry->item_guid);
                if (item)
                {
                    ItemTemplate const* prototype = item->GetTemplate();
                    if (prototype)
                    {
                        switch (prototype->Quality)
                        {
                            case 0:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_GREY_TG);
                                else
                                    config->IncItemCounts(AHB_GREY_I);
                                break;
                            case 1:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_WHITE_TG);
                                else
                                    config->IncItemCounts(AHB_WHITE_I);
                                break;
                            case 2:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_GREEN_TG);
                                else
                                    config->IncItemCounts(AHB_GREEN_I);
                                break;
                            case 3:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_BLUE_TG);
                                else
                                    config->IncItemCounts(AHB_BLUE_I);
                                break;
                            case 4:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_PURPLE_TG);
                                else
                                    config->IncItemCounts(AHB_PURPLE_I);
                                break;
                            case 5:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_ORANGE_TG);
                                else
                                    config->IncItemCounts(AHB_ORANGE_I);
                                break;
                            case 6:
                                if (prototype->Class == ITEM_CLASS_TRADE_GOODS)
                                    config->IncItemCounts(AHB_YELLOW_TG);
                                else
                                    config->IncItemCounts(AHB_YELLOW_I);
                                break;
                        }
                    }
                }
            }
        }

        if (debug_Out)
        {
            LOG_ERROR("module", "Current Settings for {} Auctionhouses:",
                      WorldDatabase.Query("SELECT name FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<std::string>());
            LOG_ERROR("module", "Grey Trade Goods\t{}\tGrey Items\t{}", config->GetItemCounts(AHB_GREY_TG), config->GetItemCounts(AHB_GREY_I));
            LOG_ERROR("module", "White Trade Goods\t{}\tWhite Items\t{}", config->GetItemCounts(AHB_WHITE_TG), config->GetItemCounts(AHB_WHITE_I));
            LOG_ERROR("module", "Green Trade Goods\t{}\tGreen Items\t{}", config->GetItemCounts(AHB_GREEN_TG), config->GetItemCounts(AHB_GREEN_I));
            LOG_ERROR("module", "Blue Trade Goods\t{}\tBlue Items\t{}", config->GetItemCounts(AHB_BLUE_TG), config->GetItemCounts(AHB_BLUE_I));
            LOG_ERROR("module", "Purple Trade Goods\t{}\tPurple Items\t{}", config->GetItemCounts(AHB_PURPLE_TG), config->GetItemCounts(AHB_PURPLE_I));
            LOG_ERROR("module", "Orange Trade Goods\t{}\tOrange Items\t{}", config->GetItemCounts(AHB_ORANGE_TG), config->GetItemCounts(AHB_ORANGE_I));
            LOG_ERROR("module", "Yellow Trade Goods\t{}\tYellow Items\t{}", config->GetItemCounts(AHB_YELLOW_TG), config->GetItemCounts(AHB_YELLOW_I));
        }
    }
    if (AHBBuyer)
    {
        //load buyer bid prices
        config->SetBuyerPrice(AHB_GREY, WorldDatabase.Query("SELECT buyerpricegrey FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetBuyerPrice(AHB_WHITE, WorldDatabase.Query("SELECT buyerpricewhite FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                             config->GetAHID())->Fetch()->Get<uint32>());
        config->SetBuyerPrice(AHB_GREEN, WorldDatabase.Query("SELECT buyerpricegreen FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                             config->GetAHID())->Fetch()->Get<uint32>());
        config->SetBuyerPrice(AHB_BLUE, WorldDatabase.Query("SELECT buyerpriceblue FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                            config->GetAHID())->Fetch()->Get<uint32>());
        config->SetBuyerPrice(AHB_PURPLE, WorldDatabase.Query("SELECT buyerpricepurple FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        config->SetBuyerPrice(AHB_ORANGE, WorldDatabase.Query("SELECT buyerpriceorange FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        config->SetBuyerPrice(AHB_YELLOW, WorldDatabase.Query("SELECT buyerpriceyellow FROM mod_auctionhousebot WHERE auctionhouse = {}",
                                                              config->GetAHID())->Fetch()->Get<uint32>());
        //load bidding interval
        config->SetBiddingInterval(
                WorldDatabase.Query("SELECT buyerbiddinginterval FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        //load bids per interval
        config->SetBidsPerInterval(
                WorldDatabase.Query("SELECT buyerbidsperinterval FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<uint32>());
        if (debug_Out)
        {
            LOG_ERROR("module", "buyerPriceGrey          = {}", config->GetBuyerPrice(AHB_GREY));
            LOG_ERROR("module", "buyerPriceWhite         = {}", config->GetBuyerPrice(AHB_WHITE));
            LOG_ERROR("module", "buyerPriceGreen         = {}", config->GetBuyerPrice(AHB_GREEN));
            LOG_ERROR("module", "buyerPriceBlue          = {}", config->GetBuyerPrice(AHB_BLUE));
            LOG_ERROR("module", "buyerPricePurple        = {}", config->GetBuyerPrice(AHB_PURPLE));
            LOG_ERROR("module", "buyerPriceOrange        = {}", config->GetBuyerPrice(AHB_ORANGE));
            LOG_ERROR("module", "buyerPriceYellow        = {}", config->GetBuyerPrice(AHB_YELLOW));
            LOG_ERROR("module", "buyerBiddingInterval    = {}", config->GetBiddingInterval());
            LOG_ERROR("module", "buyerBidsPerInterval    = {}", config->GetBidsPerInterval());
        }
    }

    if (debug_Out)
        LOG_ERROR("module", "End Settings for {} Auctionhouses:",
                  WorldDatabase.Query("SELECT name FROM mod_auctionhousebot WHERE auctionhouse = {}", config->GetAHID())->Fetch()->Get<std::string>());
}
