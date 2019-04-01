# enumivo.contracts

## Version : 1.6.0

The design of the Enumivo blockchain calls for a number of smart contracts that are run at a privileged permission level in order to support functions such as block producer registration and voting, token staking for CPU and network bandwidth, RAM purchasing, multi-sig, etc.  These smart contracts are referred to as the system, token, msig and wrap (formerly known as sudo) contracts.

This repository contains examples of these privileged contracts that are useful when deploying, managing, and/or using an Enumivo blockchain.  They are provided for reference purposes:

   * [enu.system](https://github.com/enumivo/enumivo.contracts/tree/master/enu.system)
   * [enu.msig](https://github.com/enumivo/enumivo.contracts/tree/master/enu.msig)
   * [enu.wrap](https://github.com/enumivo/enumivo.contracts/tree/master/enu.wrap)

The following unprivileged contract(s) are also part of the system.
   * [enu.token](https://github.com/enumivo/enumivo.contracts/tree/master/enu.token)

Dependencies:
* [enumivo 1.7.x](https://github.com/enumivo/enumivo/releases/tag/1.7.0)
* [enumivo.cdt 1.5.x](https://github.com/enumivo/enumivo.cdt/releases/tag/1.5.0)

To build the contracts and the unit tests:
* First, ensure that your __enumivo__ is compiled to the core symbol for the Enumivo blockchain that intend to deploy to.
* Second, make sure that you have ```sudo make install```ed __enumivo__.
* Then just run the ```build.sh``` in the top directory to build all the contracts and the unit tests for these contracts.

After build:
* The unit tests executable is placed in the _build/tests_ and is named __unit_test__.
* The contracts are built into a _bin/\<contract name\>_ folder in their respective directories.
* Finally, simply use __enucli__ to _set contract_ by pointing to the previously mentioned directory.

## License

[MIT](./LICENSE)

## Important

See LICENSE for copyright and license terms.