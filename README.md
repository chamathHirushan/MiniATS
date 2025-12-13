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
- **Encoding**: ASCII / UTF-8
- **Delimiter**: All messages must end with a newline character `\n`.
- **Arguments**: Separated by spaces.

## Client -> Server Commands

### 1. Connection & Session
*   **Command**: `LOGIN <username>`
*   **Description**: Identifies the client.
*   **Example**: `LOGIN user1`

### 2. Trading
*   **Command**: `ASK <product> <amount> <price>`
*   **Description**: Places a sell order.
*   **Example**: `ASK BTC/USD 0.5 50000`
    *   *Note*: Timestamp will be assigned by the server.

*   **Command**: `BID <product> <amount> <price>`
*   **Description**: Places a buy order.
*   **Example**: `BID BTC/USD 0.5 49000`

### 3. Information
*   **Command**: `WALLET`
*   **Description**: Request current wallet balance.
*   **Example**: `WALLET`

*   **Command**: `MARKET`
*   **Description**: Request current market statistics (high/low/volume).
*   **Example**: `MARKET`

### 4. System
*   **Command**: `EXIT`
*   **Description**: Disconnects the client.

## Server -> Client Responses

### Success
*   **Format**: `OK <optional_message>`
*   **Example**: `OK Order placed id:123`

### Error
*   **Format**: `ERR <reason>`
*   **Example**: `ERR Insufficient funds`

### Data Response (for WALLET/MARKET)
*   **Format**: `DATA <payload>`
*   **Example**: `DATA BTC:10.5,USD:500.0`


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