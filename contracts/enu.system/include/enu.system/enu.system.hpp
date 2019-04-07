/**
 *  @file
 *  @copyright defined in enumivo/LICENSE
 */
#pragma once

#include <enu.system/native.hpp>
#include <enumivolib/asset.hpp>
#include <enumivolib/time.hpp>
#include <enumivolib/privileged.hpp>
#include <enumivolib/singleton.hpp>
#include <enu.system/exchange_state.hpp>

#include <string>
#include <deque>
#include <type_traits>
#include <optional>

#ifdef CHANNEL_RAM_AND_NAMEBID_FEES_TO_REX
#undef CHANNEL_RAM_AND_NAMEBID_FEES_TO_REX
#endif
// CHANNEL_RAM_AND_NAMEBID_FEES_TO_REX macro determines whether ramfee and namebid proceeds are
// channeled to REX pool. In order to stop these proceeds from being channeled, the macro must
// be set to 0.
#define CHANNEL_RAM_AND_NAMEBID_FEES_TO_REX 1

namespace enumivosystem {

   using enumivo::name;
   using enumivo::asset;
   using enumivo::symbol;
   using enumivo::symbol_code;
   using enumivo::indexed_by;
   using enumivo::const_mem_fun;
   using enumivo::block_timestamp;
   using enumivo::time_point;
   using enumivo::time_point_sec;
   using enumivo::microseconds;
   using enumivo::datastream;
   using enumivo::check;

   template<typename E, typename F>
   static inline auto has_field( F flags, E field )
   -> std::enable_if_t< std::is_integral_v<F> && std::is_unsigned_v<F> &&
                        std::is_enum_v<E> && std::is_same_v< F, std::underlying_type_t<E> >, bool>
   {
      return ( (flags & static_cast<F>(field)) != 0 );
   }

   template<typename E, typename F>
   static inline auto set_field( F flags, E field, bool value = true )
   -> std::enable_if_t< std::is_integral_v<F> && std::is_unsigned_v<F> &&
                        std::is_enum_v<E> && std::is_same_v< F, std::underlying_type_t<E> >, F >
   {
      if( value )
         return ( flags | static_cast<F>(field) );
      else
         return ( flags & ~static_cast<F>(field) );
   }

   struct [[enumivo::table, enumivo::contract("enu.system")]] name_bid {
     name            newname;
     name            high_bidder;
     int64_t         high_bid = 0; ///< negative high_bid == closed auction waiting to be claimed
     time_point      last_bid_time;

     uint64_t primary_key()const { return newname.value;                    }
     uint64_t by_high_bid()const { return static_cast<uint64_t>(-high_bid); }
   };

   struct [[enumivo::table, enumivo::contract("enu.system")]] bid_refund {
      name         bidder;
      asset        amount;

      uint64_t primary_key()const { return bidder.value; }
   };

   typedef enumivo::multi_index< "namebids"_n, name_bid,
                               indexed_by<"highbid"_n, const_mem_fun<name_bid, uint64_t, &name_bid::by_high_bid>  >
                             > name_bid_table;

   typedef enumivo::multi_index< "bidrefunds"_n, bid_refund > bid_refund_table;

   struct [[enumivo::table("global"), enumivo::contract("enu.system")]] enumivo_global_state : enumivo::blockchain_parameters {
      uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

      uint64_t             max_ram_size = 64ll*1024 * 1024 * 1024;
      uint64_t             total_ram_bytes_reserved = 0;
      int64_t              total_ram_stake = 0;

      block_timestamp      last_producer_schedule_update;
      time_point           last_pervote_bucket_fill;
      int64_t              pervote_bucket = 0;
      int64_t              perblock_bucket = 0;
      uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
      int64_t              total_activated_stake = 0;
      time_point           thresh_activated_stake_time;
      uint16_t             last_producer_schedule_size = 0;
      double               total_producer_vote_weight = 0; /// the sum of all producer votes
      block_timestamp      last_name_close;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE_DERIVED( enumivo_global_state, enumivo::blockchain_parameters,
                                (max_ram_size)(total_ram_bytes_reserved)(total_ram_stake)
                                (last_producer_schedule_update)(last_pervote_bucket_fill)
                                (pervote_bucket)(perblock_bucket)(total_unpaid_blocks)(total_activated_stake)(thresh_activated_stake_time)
                                (last_producer_schedule_size)(total_producer_vote_weight)(last_name_close) )
   };

   /**
    * Defines new global state parameters added after version 1.0
    */
   struct [[enumivo::table("global2"), enumivo::contract("enu.system")]] enumivo_global_state2 {
      enumivo_global_state2(){}

      uint16_t          new_ram_per_block = 0;
      block_timestamp   last_ram_increase;
      block_timestamp   last_block_num; /* deprecated */
      double            total_producer_votepay_share = 0;
      uint8_t           revision = 0; ///< used to track version updates in the future.

      ENULIB_SERIALIZE( enumivo_global_state2, (new_ram_per_block)(last_ram_increase)(last_block_num)
                        (total_producer_votepay_share)(revision) )
   };

   struct [[enumivo::table("global3"), enumivo::contract("enu.system")]] enumivo_global_state3 {
      enumivo_global_state3() { }
      time_point        last_vpay_state_update;
      double            total_vpay_share_change_rate = 0;

      ENULIB_SERIALIZE( enumivo_global_state3, (last_vpay_state_update)(total_vpay_share_change_rate) )
   };

   struct [[enumivo::table, enumivo::contract("enu.system")]] producer_info {
      name                  owner;
      double                total_votes = 0;
      enumivo::public_key     producer_key; /// a packed public key object
      bool                  is_active = true;
      std::string           url;
      uint32_t              unpaid_blocks = 0;
      time_point            last_claim_time;
      uint16_t              location = 0;

      uint64_t primary_key()const { return owner.value;                             }
      double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
      bool     active()const      { return is_active;                               }
      void     deactivate()       { producer_key = public_key(); is_active = false; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( producer_info, (owner)(total_votes)(producer_key)(is_active)(url)
                        (unpaid_blocks)(last_claim_time)(location) )
   };

   struct [[enumivo::table, enumivo::contract("enu.system")]] producer_info2 {
      name            owner;
      double          votepay_share = 0;
      time_point      last_votepay_share_update;

      uint64_t primary_key()const { return owner.value; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( producer_info2, (owner)(votepay_share)(last_votepay_share_update) )
   };

   struct [[enumivo::table, enumivo::contract("enu.system")]] voter_info {
      name                owner;     /// the voter
      name                proxy;     /// the proxy set by the voter, if any
      std::vector<name>   producers; /// the producers approved by this voter if no proxy set
      int64_t             staked = 0;

      /**
       *  Every time a vote is cast we must first "undo" the last vote weight, before casting the
       *  new vote weight.  Vote weight is calculated as:
       *
       *  stated.amount * 2 ^ ( weeks_since_launch/weeks_per_year)
       */
      double              last_vote_weight = 0; /// the vote weight cast the last time the vote was updated

      /**
       * Total vote weight delegated to this voter.
       */
      double              proxied_vote_weight= 0; /// the total vote weight delegated to this voter as a proxy
      bool                is_proxy = 0; /// whether the voter is a proxy for others


      uint32_t            flags1 = 0;
      uint32_t            reserved2 = 0;
      enumivo::asset        reserved3;

      uint64_t primary_key()const { return owner.value; }

      enum class flags1_fields : uint32_t {
         ram_managed = 1,
         net_managed = 2,
         cpu_managed = 4
      };

      // explicit serialization macro is not necessary, used here only to improve compilation time
      ENULIB_SERIALIZE( voter_info, (owner)(proxy)(producers)(staked)(last_vote_weight)(proxied_vote_weight)(is_proxy)(flags1)(reserved2)(reserved3) )
   };

   typedef enumivo::multi_index< "voters"_n, voter_info >  voters_table;


   typedef enumivo::multi_index< "producers"_n, producer_info,
                               indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double, &producer_info::by_votes>  >
                             > producers_table;
   typedef enumivo::multi_index< "producers2"_n, producer_info2 > producers_table2;

   typedef enumivo::singleton< "global"_n, enumivo_global_state >   global_state_singleton;
   typedef enumivo::singleton< "global2"_n, enumivo_global_state2 > global_state2_singleton;
   typedef enumivo::singleton< "global3"_n, enumivo_global_state3 > global_state3_singleton;

   static constexpr uint32_t     seconds_per_day = 24 * 3600;

   struct [[enumivo::table,enumivo::contract("enu.system")]] rex_pool {
      uint8_t    version = 0;
      asset      total_lent; /// total amount of CORE_SYMBOL in open rex_loans
      asset      total_unlent; /// total amount of CORE_SYMBOL available to be lent (connector)
      asset      total_rent; /// fees received in exchange for lent  (connector)
      asset      total_lendable; /// total amount of CORE_SYMBOL that have been lent (total_unlent + total_lent)
      asset      total_rex; /// total number of REX shares allocated to contributors to total_lendable
      asset      namebid_proceeds; /// the amount of CORE_SYMBOL to be transferred from namebids to REX pool
      uint64_t   loan_num = 0; /// increments with each new loan

      uint64_t primary_key()const { return 0; }
   };

   typedef enumivo::multi_index< "rexpool"_n, rex_pool > rex_pool_table;

   struct [[enumivo::table,enumivo::contract("enu.system")]] rex_fund {
      uint8_t version = 0;
      name    owner;
      asset   balance;

      uint64_t primary_key()const { return owner.value; }
   };

   typedef enumivo::multi_index< "rexfund"_n, rex_fund > rex_fund_table;

   struct [[enumivo::table,enumivo::contract("enu.system")]] rex_balance {
      uint8_t version = 0;
      name    owner;
      asset   vote_stake; /// the amount of CORE_SYMBOL currently included in owner's vote
      asset   rex_balance; /// the amount of REX owned by owner
      int64_t matured_rex = 0; /// matured REX available for selling
      std::deque<std::pair<time_point_sec, int64_t>> rex_maturities; /// REX daily maturity buckets

      uint64_t primary_key()const { return owner.value; }
   };

   typedef enumivo::multi_index< "rexbal"_n, rex_balance > rex_balance_table;

   struct [[enumivo::table,enumivo::contract("enu.system")]] rex_loan {
      uint8_t             version = 0;
      name                from;
      name                receiver;
      asset               payment;
      asset               balance;
      asset               total_staked;
      uint64_t            loan_num;
      enumivo::time_point   expiration;

      uint64_t primary_key()const { return loan_num;                   }
      uint64_t by_expr()const     { return expiration.elapsed.count(); }
      uint64_t by_owner()const    { return from.value;                 }
   };

   typedef enumivo::multi_index< "cpuloan"_n, rex_loan,
                               indexed_by<"byexpr"_n,  const_mem_fun<rex_loan, uint64_t, &rex_loan::by_expr>>,
                               indexed_by<"byowner"_n, const_mem_fun<rex_loan, uint64_t, &rex_loan::by_owner>>
                             > rex_cpu_loan_table;

   typedef enumivo::multi_index< "netloan"_n, rex_loan,
                               indexed_by<"byexpr"_n,  const_mem_fun<rex_loan, uint64_t, &rex_loan::by_expr>>,
                               indexed_by<"byowner"_n, const_mem_fun<rex_loan, uint64_t, &rex_loan::by_owner>>
                             > rex_net_loan_table;

   struct [[enumivo::table,enumivo::contract("enu.system")]] rex_order {
      uint8_t             version = 0;
      name                owner;
      asset               rex_requested;
      asset               proceeds;
      asset               stake_change;
      enumivo::time_point   order_time;
      bool                is_open = true;

      void close()                { is_open = false;    }
      uint64_t primary_key()const { return owner.value; }
      uint64_t by_time()const     { return is_open ? order_time.elapsed.count() : std::numeric_limits<uint64_t>::max(); }
   };

   typedef enumivo::multi_index< "rexqueue"_n, rex_order,
                               indexed_by<"bytime"_n, const_mem_fun<rex_order, uint64_t, &rex_order::by_time>>> rex_order_table;

   struct rex_order_outcome {
      bool success;
      asset proceeds;
      asset stake_change;
   };

   class [[enumivo::contract("enu.system")]] system_contract : public native {

      private:
         voters_table            _voters;
         producers_table         _producers;
         producers_table2        _producers2;
         global_state_singleton  _global;
         global_state2_singleton _global2;
         global_state3_singleton _global3;
         enumivo_global_state      _gstate;
         enumivo_global_state2     _gstate2;
         enumivo_global_state3     _gstate3;
         rammarket               _rammarket;
         rex_pool_table          _rexpool;
         rex_fund_table          _rexfunds;
         rex_balance_table       _rexbalance;
         rex_order_table         _rexorders;

      public:
         static constexpr enumivo::name active_permission{"active"_n};
         static constexpr enumivo::name token_account{"enu.token"_n};
         static constexpr enumivo::name ram_account{"enu.ram"_n};
         static constexpr enumivo::name ramfee_account{"enu.ramfee"_n};
         static constexpr enumivo::name stake_account{"enu.stake"_n};
         static constexpr enumivo::name bpay_account{"enu.blockpay"_n};
         static constexpr enumivo::name vpay_account{"enu.votepay"_n};
         static constexpr enumivo::name names_account{"enu.names"_n};
         static constexpr enumivo::name saving_account{"enu.savings"_n};
         static constexpr enumivo::name rex_account{"enu.rex"_n};
         static constexpr enumivo::name null_account{"enumivo.null"_n};
         static constexpr symbol ramcore_symbol = symbol(symbol_code("RAMCORE"), 4);
         static constexpr symbol ram_symbol     = symbol(symbol_code("RAM"), 0);
         static constexpr symbol rex_symbol     = symbol(symbol_code("REX"), 4);

         system_contract( name s, name code, datastream<const char*> ds );
         ~system_contract();

         static symbol get_core_symbol( name system_account = "enumivo"_n ) {
            rammarket rm(system_account, system_account.value);
            const static auto sym = get_core_symbol( rm );
            return sym;
         }

         // Actions:
         [[enumivo::action]]
         void init( unsigned_int version, symbol core );
         [[enumivo::action]]
         void onblock( ignore<block_header> header );

         [[enumivo::action]]
         void setalimits( name account, int64_t ram_bytes, int64_t net_weight, int64_t cpu_weight );

         [[enumivo::action]]
         void setacctram( name account, std::optional<int64_t> ram_bytes );

         [[enumivo::action]]
         void setacctnet( name account, std::optional<int64_t> net_weight );

         [[enumivo::action]]
         void setacctcpu( name account, std::optional<int64_t> cpu_weight );

         // functions defined in delegate_bandwidth.cpp

         /**
          *  Stakes ENU from the balance of 'from' for the benfit of 'receiver'.
          *  If transfer == true, then 'receiver' can unstake to their account
          *  Else 'from' can unstake at any time.
          */
         [[enumivo::action]]
         void delegatebw( name from, name receiver,
                          asset stake_net_quantity, asset stake_cpu_quantity, bool transfer );

         /**
          * Sets total_rent balance of REX pool to the passed value
          */
         [[enumivo::action]]
         void setrex( const asset& balance );

         /**
          * Deposits core tokens to user REX fund. All proceeds and expenses related to REX are added to
          * or taken out of this fund. Inline token transfer from user balance is executed.
          */
         [[enumivo::action]]
         void deposit( const name& owner, const asset& amount );

         /**
          * Withdraws core tokens from user REX fund. Inline token transfer to user balance is
          * executed.
          */
         [[enumivo::action]]
         void withdraw( const name& owner, const asset& amount );

         /**
          * Transfers core tokens from user REX fund and converts them to REX stake.
          * A voting requirement must be satisfied before action can be executed.
          * User votes are updated following this action.
          */
         [[enumivo::action]]
         void buyrex( const name& from, const asset& amount );

         /**
          * Use staked core tokens to buy REX.
          * A voting requirement must be satisfied before action can be executed.
          * User votes are updated following this action.
          */
         [[enumivo::action]]
         void unstaketorex( const name& owner, const name& receiver, const asset& from_net, const asset& from_cpu );

         /**
          * Converts REX stake back into core tokens at current exchange rate. If order cannot be
          * processed, it gets queued until there is enough in REX pool to fill order.
          * If successful, user votes are updated.
          */
         [[enumivo::action]]
         void sellrex( const name& from, const asset& rex );

         /**
          * Cancels queued sellrex order. Order cannot be cancelled once it's been filled.
          */
         [[enumivo::action]]
         void cnclrexorder( const name& owner );

         /**
          * Use payment to rent as many ENU tokens as possible and stake them for either CPU or NET for the
          * benefit of receiver, after 30 days the rented ENU delegation of CPU or NET will expire unless loan
          * balance is larger than or equal to payment.
          *
          * If loan has enough balance, it gets renewed at current market price, otherwise, it is closed and
          * remaining balance is refunded to loan owner.
          *
          * Owner can fund or defund a loan at any time before its expiration.
          *
          * All loan expenses and refunds come out of or are added to owner's REX fund.
          */
         [[enumivo::action]]
         void rentcpu( const name& from, const name& receiver, const asset& loan_payment, const asset& loan_fund );
         [[enumivo::action]]
         void rentnet( const name& from, const name& receiver, const asset& loan_payment, const asset& loan_fund );

         /**
          * Loan owner funds a given CPU or NET loan.
          */
         [[enumivo::action]]
         void fundcpuloan( const name& from, uint64_t loan_num, const asset& payment );
         [[enumivo::action]]
         void fundnetloan( const name& from, uint64_t loan_num, const asset& payment );
         /**
          * Loan owner defunds a given CPU or NET loan.
          */
         [[enumivo::action]]
         void defcpuloan( const name& from, uint64_t loan_num, const asset& amount );
         [[enumivo::action]]
         void defnetloan( const name& from, uint64_t loan_num, const asset& amount );

         /**
          * Updates REX vote stake of owner to its current value.
          */
         [[enumivo::action]]
         void updaterex( const name& owner );

         /**
          * Processes max CPU loans, max NET loans, and max queued sellrex orders.
          * Action does not execute anything related to a specific user.
          */
         [[enumivo::action]]
         void rexexec( const name& user, uint16_t max );

         /**
          * Consolidate REX maturity buckets into one that can be sold only 4 days
          * from the end of today.
          */
         [[enumivo::action]]
         void consolidate( const name& owner );

         /**
          * Moves a specified amount of REX into savings bucket. REX savings bucket
          * never matures. In order for it to be sold, it has to be moved explicitly
          * out of that bucket. Then the moved amount will have the regular maturity
          * period of 4 days starting from the end of the day.
          */
         [[enumivo::action]]
         void mvtosavings( const name& owner, const asset& rex );
         
         /**
          * Moves a specified amount of REX out of savings bucket. The moved amount
          * will have the regular REX maturity period of 4 days.  
          */
         [[enumivo::action]]
         void mvfrsavings( const name& owner, const asset& rex );

         /**
          * Deletes owner records from REX tables and frees used RAM.
          * Owner must not have an outstanding REX balance.
          */
         [[enumivo::action]]
         void closerex( const name& owner );

         /**
          *  Decreases the total tokens delegated by from to receiver and/or
          *  frees the memory associated with the delegation if there is nothing
          *  left to delegate.
          *
          *  This will cause an immediate reduction in net/cpu bandwidth of the
          *  receiver.
          *
          *  A transaction is scheduled to send the tokens back to 'from' after
          *  the staking period has passed. If existing transaction is scheduled, it
          *  will be canceled and a new transaction issued that has the combined
          *  undelegated amount.
          *
          *  The 'from' account loses voting power as a result of this call and
          *  all producer tallies are updated.
          */
         [[enumivo::action]]
         void undelegatebw( name from, name receiver,
                            asset unstake_net_quantity, asset unstake_cpu_quantity );


         /**
          * Increases receiver's ram quota based upon current price and quantity of
          * tokens provided. An inline transfer from receiver to system contract of
          * tokens will be executed.
          */
         [[enumivo::action]]
         void buyram( name payer, name receiver, asset quant );
         [[enumivo::action]]
         void buyrambytes( name payer, name receiver, uint32_t bytes );

         /**
          *  Reduces quota my bytes and then performs an inline transfer of tokens
          *  to receiver based upon the average purchase price of the original quota.
          */
         [[enumivo::action]]
         void sellram( name account, int64_t bytes );

         /**
          *  This action is called after the delegation-period to claim all pending
          *  unstaked tokens belonging to owner
          */
         [[enumivo::action]]
         void refund( name owner );

         // functions defined in voting.cpp

         [[enumivo::action]]
         void regproducer( const name producer, const public_key& producer_key, const std::string& url, uint16_t location );

         [[enumivo::action]]
         void unregprod( const name producer );

         [[enumivo::action]]
         void setram( uint64_t max_ram_size );
         [[enumivo::action]]
         void setramrate( uint16_t bytes_per_block );

         [[enumivo::action]]
         void voteproducer( const name voter, const name proxy, const std::vector<name>& producers );

         [[enumivo::action]]
         void regproxy( const name proxy, bool isproxy );

         [[enumivo::action]]
         void setparams( const enumivo::blockchain_parameters& params );

         // functions defined in producer_pay.cpp
         [[enumivo::action]]
         void claimrewards( const name owner );

         [[enumivo::action]]
         void setpriv( name account, uint8_t is_priv );

         [[enumivo::action]]
         void rmvproducer( name producer );

         [[enumivo::action]]
         void updtrevision( uint8_t revision );

         [[enumivo::action]]
         void bidname( name bidder, name newname, asset bid );

         [[enumivo::action]]
         void bidrefund( name bidder, name newname );

         using init_action = enumivo::action_wrapper<"init"_n, &system_contract::init>;
         using setacctram_action = enumivo::action_wrapper<"setacctram"_n, &system_contract::setacctram>;
         using setacctnet_action = enumivo::action_wrapper<"setacctnet"_n, &system_contract::setacctnet>;
         using setacctcpu_action = enumivo::action_wrapper<"setacctcpu"_n, &system_contract::setacctcpu>;
         using delegatebw_action = enumivo::action_wrapper<"delegatebw"_n, &system_contract::delegatebw>;
         using deposit_action = enumivo::action_wrapper<"deposit"_n, &system_contract::deposit>;
         using withdraw_action = enumivo::action_wrapper<"withdraw"_n, &system_contract::withdraw>;
         using buyrex_action = enumivo::action_wrapper<"buyrex"_n, &system_contract::buyrex>;
         using unstaketorex_action = enumivo::action_wrapper<"unstaketorex"_n, &system_contract::unstaketorex>;
         using sellrex_action = enumivo::action_wrapper<"sellrex"_n, &system_contract::sellrex>;
         using cnclrexorder_action = enumivo::action_wrapper<"cnclrexorder"_n, &system_contract::cnclrexorder>;
         using rentcpu_action = enumivo::action_wrapper<"rentcpu"_n, &system_contract::rentcpu>;
         using rentnet_action = enumivo::action_wrapper<"rentnet"_n, &system_contract::rentnet>;
         using fundcpuloan_action = enumivo::action_wrapper<"fundcpuloan"_n, &system_contract::fundcpuloan>;
         using fundnetloan_action = enumivo::action_wrapper<"fundnetloan"_n, &system_contract::fundnetloan>;
         using defcpuloan_action = enumivo::action_wrapper<"defcpuloan"_n, &system_contract::defcpuloan>;
         using defnetloan_action = enumivo::action_wrapper<"defnetloan"_n, &system_contract::defnetloan>;
         using updaterex_action = enumivo::action_wrapper<"updaterex"_n, &system_contract::updaterex>;
         using rexexec_action = enumivo::action_wrapper<"rexexec"_n, &system_contract::rexexec>;
         using setrex_action = enumivo::action_wrapper<"setrex"_n, &system_contract::setrex>;
         using mvtosavings_action = enumivo::action_wrapper<"mvtosavings"_n, &system_contract::mvtosavings>;
         using mvfrsavings_action = enumivo::action_wrapper<"mvfrsavings"_n, &system_contract::mvfrsavings>;
         using consolidate_action = enumivo::action_wrapper<"consolidate"_n, &system_contract::consolidate>;
         using closerex_action = enumivo::action_wrapper<"closerex"_n, &system_contract::closerex>;
         using undelegatebw_action = enumivo::action_wrapper<"undelegatebw"_n, &system_contract::undelegatebw>;
         using buyram_action = enumivo::action_wrapper<"buyram"_n, &system_contract::buyram>;
         using buyrambytes_action = enumivo::action_wrapper<"buyrambytes"_n, &system_contract::buyrambytes>;
         using sellram_action = enumivo::action_wrapper<"sellram"_n, &system_contract::sellram>;
         using refund_action = enumivo::action_wrapper<"refund"_n, &system_contract::refund>;
         using regproducer_action = enumivo::action_wrapper<"regproducer"_n, &system_contract::regproducer>;
         using unregprod_action = enumivo::action_wrapper<"unregprod"_n, &system_contract::unregprod>;
         using setram_action = enumivo::action_wrapper<"setram"_n, &system_contract::setram>;
         using setramrate_action = enumivo::action_wrapper<"setramrate"_n, &system_contract::setramrate>;
         using voteproducer_action = enumivo::action_wrapper<"voteproducer"_n, &system_contract::voteproducer>;
         using regproxy_action = enumivo::action_wrapper<"regproxy"_n, &system_contract::regproxy>;
         using claimrewards_action = enumivo::action_wrapper<"claimrewards"_n, &system_contract::claimrewards>;
         using rmvproducer_action = enumivo::action_wrapper<"rmvproducer"_n, &system_contract::rmvproducer>;
         using updtrevision_action = enumivo::action_wrapper<"updtrevision"_n, &system_contract::updtrevision>;
         using bidname_action = enumivo::action_wrapper<"bidname"_n, &system_contract::bidname>;
         using bidrefund_action = enumivo::action_wrapper<"bidrefund"_n, &system_contract::bidrefund>;
         using setpriv_action = enumivo::action_wrapper<"setpriv"_n, &system_contract::setpriv>;
         using setalimits_action = enumivo::action_wrapper<"setalimits"_n, &system_contract::setalimits>;
         using setparams_action = enumivo::action_wrapper<"setparams"_n, &system_contract::setparams>;

      private:

         // Implementation details:

         static symbol get_core_symbol( const rammarket& rm ) {
            auto itr = rm.find(ramcore_symbol.raw());
            check(itr != rm.end(), "system contract must first be initialized");
            return itr->quote.balance.symbol;
         }

         //defined in enu.system.cpp
         static enumivo_global_state get_default_parameters();
         static time_point current_time_point();
         static time_point_sec current_time_point_sec();
         static block_timestamp current_block_time();
         symbol core_symbol()const;
         void update_ram_supply();

         // defined in rex.cpp
         void runrex( uint16_t max );
         void update_resource_limits( const name& from, const name& receiver, int64_t delta_net, int64_t delta_cpu );
         void check_voting_requirement( const name& owner,
                                        const char* error_msg = "must vote for at least 21 producers or for a proxy before buying REX" )const;
         rex_order_outcome fill_rex_order( const rex_balance_table::const_iterator& bitr, const asset& rex );
         asset update_rex_account( const name& owner, const asset& proceeds, const asset& unstake_quant, bool force_vote_update = false );
         void channel_to_rex( const name& from, const asset& amount );
         void channel_namebid_to_rex( const int64_t highest_bid );
         template <typename T>
         int64_t rent_rex( T& table, const name& from, const name& receiver, const asset& loan_payment, const asset& loan_fund );
         template <typename T>
         void fund_rex_loan( T& table, const name& from, uint64_t loan_num, const asset& payment );
         template <typename T>
         void defund_rex_loan( T& table, const name& from, uint64_t loan_num, const asset& amount );
         void transfer_from_fund( const name& owner, const asset& amount );
         void transfer_to_fund( const name& owner, const asset& amount );
         bool rex_loans_available()const;
         bool rex_system_initialized()const { return _rexpool.begin() != _rexpool.end(); }
         bool rex_available()const { return rex_system_initialized() && _rexpool.begin()->total_rex.amount > 0; }
         static time_point_sec get_rex_maturity();
         asset add_to_rex_balance( const name& owner, const asset& payment, const asset& rex_received );
         asset add_to_rex_pool( const asset& payment );
         void process_rex_maturities( const rex_balance_table::const_iterator& bitr );
         void consolidate_rex_balance( const rex_balance_table::const_iterator& bitr,
                                       const asset& rex_in_sell_order );
         int64_t read_rex_savings( const rex_balance_table::const_iterator& bitr );
         void put_rex_savings( const rex_balance_table::const_iterator& bitr, int64_t rex );
         void update_rex_stake( const name& voter );

         void add_loan_to_rex_pool( const asset& payment, int64_t rented_tokens, bool new_loan );
         void remove_loan_from_rex_pool( const rex_loan& loan );
         template <typename Index, typename Iterator>
         int64_t update_renewed_loan( Index& idx, const Iterator& itr, int64_t rented_tokens );

         // defined in delegate_bandwidth.cpp
         void changebw( name from, name receiver,
                        asset stake_net_quantity, asset stake_cpu_quantity, bool transfer );
         void update_voting_power( const name& voter, const asset& total_update );

         // defined in voting.hpp
         void update_elected_producers( block_timestamp timestamp );
         void update_votes( const name voter, const name proxy, const std::vector<name>& producers, bool voting );
         void propagate_weight_change( const voter_info& voter );
         double update_producer_votepay_share( const producers_table2::const_iterator& prod_itr,
                                               time_point ct,
                                               double shares_rate, bool reset_to_zero = false );
         double update_total_votepay_share( time_point ct,
                                            double additional_shares_delta = 0.0, double shares_rate_delta = 0.0 );

         template <auto system_contract::*...Ptrs>
         class registration {
            public:
               template <auto system_contract::*P, auto system_contract::*...Ps>
               struct for_each {
                  template <typename... Args>
                  static constexpr void call( system_contract* this_contract, Args&&... args )
                  {
                     std::invoke( P, this_contract, std::forward<Args>(args)... );
                     for_each<Ps...>::call( this_contract, std::forward<Args>(args)... );
                  }
               };
               template <auto system_contract::*P>
               struct for_each<P> {
                  template <typename... Args>
                  static constexpr void call( system_contract* this_contract, Args&&... args )
                  {
                     std::invoke( P, this_contract, std::forward<Args>(args)... );
                  }
               };

               template <typename... Args>
               constexpr void operator() ( Args&&... args )
               {
                  for_each<Ptrs...>::call( this_contract, std::forward<Args>(args)... );
               }

               system_contract* this_contract;
         };

         registration<&system_contract::update_rex_stake> vote_stake_updater{ this };
   };

} /// enumivosystem
