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
    placeOrder: async (order: { product: string, price: number, amount: number, type: string, username: string }) => {
        const res = await fetch(`${API_BASE}/order`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(order),
        });
        return res;
    }
};
