#pragma once

#include <enumivolib/enumivo.hpp>
#include <enumivolib/ignore.hpp>
#include <enumivolib/transaction.hpp>

namespace enumivo {

   class [[enumivo::contract("enu.wrap")]] wrap : public contract {
      public:
         using contract::contract;

         [[enumivo::action]]
         void exec( ignore<name> executer, ignore<transaction> trx );

   };

} /// namespace enumivo
