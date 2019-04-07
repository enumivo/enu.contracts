#pragma once
#include <enumivolib/action.hpp>
#include <enumivolib/crypto.h>
#include <enumivolib/enu.hpp>
#include <enumivolib/privileged.hpp>
#include <enumivolib/producer_schedule.hpp>

namespace enumivo {
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
      std::optional<enumivo::producer_schedule>   new_producers;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE(block_header, (timestamp)(producer)(confirmed)(previous)(transaction_mroot)(action_mroot)
                                     (schedule_version)(new_producers))
   };

   class [[enumivo::contract("enu.bios")]] bios : public contract {
      public:
         using contract::contract;
         [[enumivo::action]]
         void newaccount( name             creator,
                          name             name,
                          ignore<authority> owner,
                          ignore<authority> active){}


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
         void setcode( name account, uint8_t vmtype, uint8_t vmversion, const std::vector<char>& code ) {}

         [[enumivo::action]]
         void setpriv( name account, uint8_t is_priv ) {
            require_auth( _self );
            set_privileged( account.value, is_priv );
         }

         [[enumivo::action]]
         void setalimits( name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight ) {
            require_auth( _self );
            set_resource_limits( account.value, ram_bytes, net_weight, cpu_weight );
         }

         [[enumivo::action]]
         void setglimits( uint64_t ram, uint64_t net, uint64_t cpu ) {
            (void)ram; (void)net; (void)cpu;
            require_auth( _self );
         }

         [[enumivo::action]]
         void setprods( std::vector<enumivo::producer_key> schedule ) {
            (void)schedule; // schedule argument just forces the deserialization of the action data into vector<producer_key> (necessary check)
            require_auth( _self );

            constexpr size_t max_stack_buffer_size = 512;
            size_t size = action_data_size();
            char* buffer = (char*)( max_stack_buffer_size < size ? malloc(size) : alloca(size) );
            read_action_data( buffer, size );
            set_proposed_producers(buffer, size);
         }

         [[enumivo::action]]
         void setparams( const enumivo::blockchain_parameters& params ) {
            require_auth( _self );
            set_blockchain_parameters( params );
         }

         [[enumivo::action]]
         void reqauth( name from ) {
            require_auth( from );
         }

         [[enumivo::action]]
         void setabi( name account, const std::vector<char>& abi ) {
            abi_hash_table table(_self, _self.value);
            auto itr = table.find( account.value );
            if( itr == table.end() ) {
               table.emplace( account, [&]( auto& row ) {
                  row.owner = account;
                  sha256( const_cast<char*>(abi.data()), abi.size(), &row.hash );
               });
            } else {
               table.modify( itr, same_payer, [&]( auto& row ) {
                  sha256( const_cast<char*>(abi.data()), abi.size(), &row.hash );
               });
            }
         }

         struct [[enumivo::table]] abi_hash {
            name              owner;
            capi_checksum256  hash;
            uint64_t primary_key()const { return owner.value; }

            ENULIB_SERIALIZE( abi_hash, (owner)(hash) )
         };

         typedef enumivo::multi_index< "abihash"_n, abi_hash > abi_hash_table;
         
         using newaccount_action = action_wrapper<"newaccount"_n, &bios::newaccount>;
         using updateauth_action = action_wrapper<"updateauth"_n, &bios::updateauth>;
         using deleteauth_action = action_wrapper<"deleteauth"_n, &bios::deleteauth>;
         using linkauth_action = action_wrapper<"linkauth"_n, &bios::linkauth>;
         using unlinkauth_action = action_wrapper<"unlinkauth"_n, &bios::unlinkauth>;
         using canceldelay_action = action_wrapper<"canceldelay"_n, &bios::canceldelay>;
         using setcode_action = action_wrapper<"setcode"_n, &bios::setcode>;
         using setpriv_action = action_wrapper<"setpriv"_n, &bios::setpriv>;
         using setalimits_action = action_wrapper<"setalimits"_n, &bios::setalimits>;
         using setglimits_action = action_wrapper<"setglimits"_n, &bios::setglimits>;
         using setprods_action = action_wrapper<"setprods"_n, &bios::setprods>;
         using setparams_action = action_wrapper<"setparams"_n, &bios::setparams>;
         using reqauth_action = action_wrapper<"reqauth"_n, &bios::reqauth>;
         using setabi_action = action_wrapper<"setabi"_n, &bios::setabi>;
   };

} /// namespace enumivo
