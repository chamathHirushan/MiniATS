# MiniATS

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

## Compilation

To compile the project using `g++`, use the following command:

```bash
make all
```

Feel free to edit the `Makefile` to address any cross-platform issues.

## Trading System Protocol Design
- **Encoding**: 
- **Delimiter**: All messages must end with a newline character `\n`.
- **Arguments**: Separated by spaces.

### Client -> Server Commands

1. **Login:**  
    Use `LOGIN <username>` to identify yourself to the server.

2. **Place Orders:**  
    - To sell, use `ASK <product> <amount> <price>`.  
        <sub>Example: <code>ASK BTC/USD 0.5 50000</code>  
        (The server adds a timestamp automatically.)</sub>
    - To buy, use `BID <product> <amount> <price>`.  
        <sub>Example: <code>BID BTC/USD 0.5 49000</code></sub>

3. **Get Information:**  
    - Check your wallet with `WALLET`.
    - View market stats using `MARKET`.
    - View commands `HELP`.

4. **Disconnect:**  
    Use `EXIT` to log out from the server.

---

### Server -> Client Responses

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