# MiniATS

## Description

A mini trading system that enables users to place BID and ASK orders through a client–server architecture, allowing multiple clients to trade concurrently. The system includes a price–time priority order matching engine that matches buy and sell orders efficiently, along with a wallet management system to handle user balances, funds reservation, and transaction settlements. At the end of each trading session, the system exports market data, including filled and remaining orders, enabling state persistence and reuse in subsequent sessions.

## Compilation

To compile the project using `g++`, use the following command:

```bash
make all
```
Feel free to edit the `Makefile` to address any cross-platform issues.

### Running the Server and Clients

Start the server and any number of clients using the following commands:

```bash
build/server

build/client
```

## Client -> Server Commands

1. **Register**
   Use `REGISTER <username> <password>` to register to the platform.

2. **Login:**  
    Use `LOGIN <username> <password>` to identify yourself to the server.

3. **Place Orders:**  
    - To sell, use `ASK <product> <amount> <price>`.  
        <sub>Example: <code>ASK BTC/USD 0.5 50000</code> (The server adds a timestamp automatically.)</sub>
    - To buy, use `BID <product> <amount> <price>`.  
        <sub>Example: <code>BID BTC/USD 0.5 49000</code></sub>

3. **Get Information:**  
    - Check your wallet with `WALLET`.
    - View market stats using `MARKET`.
    - View product statistics `STATS <product>`.
    - View commands `HELP`.

4. **Manage Wallet:**  
    - To Deposit, use `DEPOSIT <product> <amount>`.  
        <sub>Example: <code>DEPOSIT USDT 1000</code></sub>
    - To Withdraw, use `WITHDRAW <product> <amount>`.  
        <sub>Example: <code>WITHDRAW ETH 20</code></sub>

5. **Manage Orders:**  
    - View open orders with `ORDERS`.
    - To Cancel an order, use `CANCEL <order id>`.    
        <sub>Example: <code>CANCEL 3500</code></sub>

6. **Disconnect:**  
    Use `EXIT` to log out from the server.

---

## Server -> Client Responses

- Successful actions return `OK` followed by a message, like `OK Order placed id:123`.
- Errors are sent as `ERR <reason>`, for example, `ERR Insufficient funds`.
- Data requests (like wallet or market info) respond with `DATA <payload>`, such as `DATA BTC:10.5,USD:500.0`.


## Matching Algorithm

 ```bash
 asks = orderbook.asks in this timeframe
 bids = orderbook.bids in this timeframe

 sales = []
 
 sort asks lowest first
 sort bids highest first

 for ask in asks:
    for bid in bids:
        if bid.price >= ask.price # we have a match
            sale = new orderbookentry()
            sale.price = ask.price
            if bid.amount == ask.amount: # bid completely clears ask
                sale.amount = ask.amount
                sales.append(sale)
                bid.amount = 0 # make sure the bid is not processed again
                ask.amount = 0
                # can do no more with this ask
                # go onto the next ask
                break
            if bid.amount > ask.amount: # ask is completely gone slice the bid
                sale.amount = ask.amount
                sales.append(sale)
                # we adjust the bid in place
                # so it can be used to process the next ask
                bid.amount = bid.amount- ask.amount
                ask.amount = 0
                # ask is completely gone, so go to next ask
                break
            if bid.amount < ask.amount # bid is completely gone, slice the ask
                sale.amount = bid.amount
                sales.append(sale)
                # update the ask
                # and allow further bids to process the remaining amount
                ask.amount = ask.amount- bid.amount
                bid.amount = 0 # make sure the bid is not processed again
                # some ask remains so go to the next bid
                continue
 return sales
 ```

## Download ASIO for Cross-Platform Client-Server

1. Download ASIO from [the official SourceForge page](https://sourceforge.net/projects/asio/).
2. Extract the downloaded archive.
3. Move the extracted folder to the `external` directory at the root of the project.
4. Rename the folder to `asio` so the structure is:  
    ```
    root/
      external/
         asio/
    ```
