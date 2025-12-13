# MiniATS

# Download asio for cross platform client-server
```bash
https://sourceforge.net/projects/asio/
````

## Compilation

To compile the project using g++, use the following command:

```bash
g++ -std=c++17 src/*.cpp -I include -o build/app
```

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