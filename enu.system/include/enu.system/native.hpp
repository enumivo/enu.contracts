/**
 *  @file
 *  @copyright defined in enumivo/LICENSE
 */
#pragma once

#include <enumivolib/action.hpp>
#include <enumivolib/public_key.hpp>
#include <enumivolib/print.hpp>
#include <enumivolib/privileged.h>
#include <enumivolib/producer_schedule.hpp>
#include <enumivolib/contract.hpp>
#include <enumivolib/ignore.hpp>

namespace enumivosystem {
   using enumivo::name;
   using enumivo::permission_level;
   using enumivo::public_key;
   using enumivo::ignore;

   struct permission_level_weight {
      permission_level  permission;
      uint16_t          weight;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( permission_level_weight, (permission)(weight) )
   };

   struct key_weight {
      enumivo::public_key  key;
      uint16_t           weight;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( key_weight, (key)(weight) )
   };

   struct wait_weight {
      uint32_t           wait_sec;
      uint16_t           weight;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( wait_weight, (wait_sec)(weight) )
   };

   struct authority {
      uint32_t                              threshold = 0;
      std::vector<key_weight>               keys;
      std::vector<permission_level_weight>  accounts;
      std::vector<wait_weight>              waits;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( authority, (threshold)(keys)(accounts)(waits) )
   };

   struct block_header {
      uint32_t                                  timestamp;
      name                                      producer;
      uint16_t                                  confirmed = 0;
      capi_checksum256                          previous;
      capi_checksum256                          transaction_mroot;
      capi_checksum256                          action_mroot;
      uint32_t                                  schedule_version = 0;
      std::optional<enumivo::producer_schedule> new_producers;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE(block_header, (timestamp)(producer)(confirmed)(previous)(transaction_mroot)(action_mroot)
                                     (schedule_version)(new_producers))
   };


   struct [[enumivo::table("abihash"), enumivo::contract("enu.system")]] abi_hash {
      name              owner;
      capi_checksum256  hash;
      uint64_t primary_key()const { return owner.value; }

      ENULIB_SERIALIZE( abi_hash, (owner)(hash) )
   };

   /*
    * Method parameters commented out to prevent generation of code that parses input data.
    */
   class [[enumivo::contract("enu.system")]] native : public enumivo::contract {
      public:

         using enumivo::contract::contract;

         /**
          *  Called after a new account is created. This code enforces resource-limits rules
          *  for new accounts as well as new account naming conventions.
          *
          *  1. accounts cannot contain '.' symbols which forces all acccounts to be 12
          *  characters long without '.' until a future account auction process is implemented
          *  which prevents name squatting.
          *
          *  2. new accounts must stake a minimal number of tokens (as set in system parameters)
          *     therefore, this method will execute an inline buyram from receiver for newacnt in
          *     an amount equal to the current new account creation fee.
          */
         [[enumivo::action]]
         void newaccount( name             creator,
                          name             name,
                          ignore<authority> owner,
                          ignore<authority> active);


         [[enumivo::action]]
         void updateauth(  ignore<name>  account,
                           ignore<name>  permission,
                           ignore<name>  parent,
                           ignore<authority> auth ) {}

         [[enumivo::action]]
         void deleteauth( ignore<name>  account,
                          ignore<name>  permission ) {}

         [[enumivo::action]]
         void linkauth(  ignore<name>    account,
                         ignore<name>    code,
                         ignore<name>    type,
                         ignore<name>    requirement  ) {}

         [[enumivo::action]]
         void unlinkauth( ignore<name>  account,
                          ignore<name>  code,
                          ignore<name>  type ) {}

         [[enumivo::action]]
         void canceldelay( ignore<permission_level> canceling_auth, ignore<capi_checksum256> trx_id ) {}

         [[enumivo::action]]
         void onerror( ignore<uint128_t> sender_id, ignore<std::vector<char>> sent_trx ) {}

         [[enumivo::action]]
         void setabi( name account, const std::vector<char>& abi );

         [[enumivo::action]]
         void setcode( name account, uint8_t vmtype, uint8_t vmversion, const std::vector<char>& code ) {}
   };
}
