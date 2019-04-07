#pragma once

#include <enulib/enu.hpp>
#include <enulib/name.hpp>
#include <enulib/asset.hpp>

using enumivo::name;
using enumivo::asset;
using enumivo::action_wrapper;

class [[enumivo::contract("rex.results")]] rex_results : enumivo::contract {
   public:

      using enumivo::contract::contract;

      [[enumivo::action]]
      void buyresult( const asset& rex_received );

      [[enumivo::action]]
      void sellresult( const asset& proceeds );

      [[enumivo::action]]
      void orderresult( const name& owner, const asset& proceeds );

      [[enumivo::action]]
      void rentresult( const asset& rented_tokens );

      using buyresult_action   = action_wrapper<"buyresult"_n,   &rex_results::buyresult>;
      using sellresult_action  = action_wrapper<"sellresult"_n,  &rex_results::sellresult>;
      using orderresult_action = action_wrapper<"orderresult"_n, &rex_results::orderresult>;
      using rentresult_action  = action_wrapper<"rentresult"_n,  &rex_results::rentresult>;
};
