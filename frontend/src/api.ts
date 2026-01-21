// api.ts
export const API_BASE = "http://localhost:18080";

export interface Order {
    id: number;
    price: number;
    amount: number;
    timestamp: string;
    product: string;
    type: string;
    username: string;
}

export const api = {
    getHealth: async () => {
        const res = await fetch(`${API_BASE}/health`);
        return res.text();
    },
    getProducts: async () => {
        const res = await fetch(`${API_BASE}/products`);
        return res.json() as Promise<string[]>;
    },
    getOrders: async (product: string) => {
        const res = await fetch(`${API_BASE}/orders?product=${encodeURIComponent(product)}`);
        return res.json() as Promise<Order[]>;
    },
    getUserOrders: async (username: string) => {
        const res = await fetch(`${API_BASE}/user/orders?username=${encodeURIComponent(username)}`);
        return res.json() as Promise<Order[]>;
    },
    getWallet: async (username: string) => {
        const res = await fetch(`${API_BASE}/wallet?username=${encodeURIComponent(username)}`);
        return res.json();
    },
    deposit: async (data: {username: string, product: string, amount: number}) => {
        const res = await fetch(`${API_BASE}/wallet/deposit`, {
             method: 'POST',
             headers: {'Content-Type': 'application/json'},
             body: JSON.stringify(data)
        });
        if (res.status !== 200) throw new Error("Deposit failed");
        return true;
    },
    withdraw: async (data: {username: string, product: string, amount: number}) => {
        const res = await fetch(`${API_BASE}/wallet/withdraw`, {
             method: 'POST',
             headers: {'Content-Type': 'application/json'},
             body: JSON.stringify(data)
        });
        if (res.status !== 200) throw new Error("Withdraw failed");
        return true;
    },
    login: async (creds: {username: string, password: string}) => {
        const res = await fetch(`${API_BASE}/login`, {
             method: 'POST',
             headers: {'Content-Type': 'application/json'},
             body: JSON.stringify(creds)
        });
        if (res.status === 200) return true;
        throw new Error("Login failed");
    },
    register: async (creds: {username: string, password: string}) => {
        const res = await fetch(`${API_BASE}/register`, {
             method: 'POST',
             headers: {'Content-Type': 'application/json'},
             body: JSON.stringify(creds)
        });
        if (res.status === 201) return true;
        throw new Error("Registration failed");
    },
    cancelOrder: async (id: number, username: string) => {
        const res = await fetch(`${API_BASE}/order?id=${id}&username=${username}`, {
            method: 'DELETE'
        });
        if (res.status !== 200) throw new Error("Correction failed");
        return true;
    },
    placeOrder: async (order: { product: string, price: number, amount: number, type: string, username: string }) => {
        const res = await fetch(`${API_BASE}/order`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(order),
        });
        if (res.status !== 201) throw new Error('Order failed');
        return res;
    }
};
