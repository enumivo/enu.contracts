#pragma once
#include <enumivolib/enu.hpp>
#include <enumivolib/ignore.hpp>
#include <enumivolib/transaction.hpp>

namespace enumivo {

   class [[enumivo::contract("enu.msig")]] multisig : public contract {
      public:
         using contract::contract;

         [[enumivo::action]]
         void propose(ignore<name> proposer, ignore<name> proposal_name,
               ignore<std::vector<permission_level>> requested, ignore<transaction> trx);
         [[enumivo::action]]
         void approve( name proposer, name proposal_name, permission_level level,
                       const enumivo::binary_extension<enumivo::checksum256>& proposal_hash );
         [[enumivo::action]]
         void unapprove( name proposer, name proposal_name, permission_level level );
         [[enumivo::action]]
         void cancel( name proposer, name proposal_name, name canceler );
         [[enumivo::action]]
         void exec( name proposer, name proposal_name, name executer );
         [[enumivo::action]]
         void invalidate( name account );

         using propose_action = enumivo::action_wrapper<"propose"_n, &multisig::propose>;
         using approve_action = enumivo::action_wrapper<"approve"_n, &multisig::approve>;
         using unapprove_action = enumivo::action_wrapper<"unapprove"_n, &multisig::unapprove>;
         using cancel_action = enumivo::action_wrapper<"cancel"_n, &multisig::cancel>;
         using exec_action = enumivo::action_wrapper<"exec"_n, &multisig::exec>;
         using invalidate_action = enumivo::action_wrapper<"invalidate"_n, &multisig::invalidate>;
      private:
         struct [[enumivo::table]] proposal {
            name                            proposal_name;
            std::vector<char>               packed_transaction;

            uint64_t primary_key()const { return proposal_name.value; }
         };

         typedef enumivo::multi_index< "proposal"_n, proposal > proposals;

         struct [[enumivo::table]] old_approvals_info {
            name                            proposal_name;
            std::vector<permission_level>   requested_approvals;
            std::vector<permission_level>   provided_approvals;

            uint64_t primary_key()const { return proposal_name.value; }
         };
         typedef enumivo::multi_index< "approvals"_n, old_approvals_info > old_approvals;

         struct approval {
            permission_level level;
            time_point       time;
         };

         struct [[enumivo::table]] approvals_info {
            uint8_t                 version = 1;
            name                    proposal_name;
            //requested approval doesn't need to cointain time, but we want requested approval
            //to be of exact the same size ad provided approval, in this case approve/unapprove
            //doesn't change serialized data size. So, we use the same type.
            std::vector<approval>   requested_approvals;
            std::vector<approval>   provided_approvals;

            uint64_t primary_key()const { return proposal_name.value; }
         };
         typedef enumivo::multi_index< "approvals2"_n, approvals_info > approvals;

         struct [[enumivo::table]] invalidation {
            name         account;
            time_point   last_invalidation_time;

            uint64_t primary_key() const { return account.value; }
         };

         typedef enumivo::multi_index< "invals"_n, invalidation > invalidations;
   };

} /// namespace enumivo
