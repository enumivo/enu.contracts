#pragma once

#include <enulib/enu.hpp>
#include <enulib/ignore.hpp>
#include <enulib/transaction.hpp>

namespace enumivo {

   class [[enumivo::contract("enu.wrap")]] wrap : public contract {
      public:
         using contract::contract;

         [[enumivo::action]]
         void exec( ignore<name> executer, ignore<transaction> trx );

         using exec_action = enumivo::action_wrapper<"exec"_n, &wrap::exec>;
   };

} /// namespace enumivo
