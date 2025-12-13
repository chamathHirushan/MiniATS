# Trading System Protocol Design

This document defines the text-based communication protocol between the Trading Client and the Trading Server.

## General Format
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
