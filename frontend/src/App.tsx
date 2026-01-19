import { useState, useEffect } from 'react'
import './App.css'
import { api } from './api'
import type { Order } from './api'

// type View = 'trading' | 'market' | 'orders'

function App() {
  // const [currentView] = useState<View>('market')
  
  // Data
  const [products, setProducts] = useState<string[]>([])
  const [orders, setOrders] = useState<Order[]>([])
  const [health, setHealth] = useState<string>('Unknown')
  
  // Forms
  const [selectedProduct, setSelectedProduct] = useState('')
  const [price, setPrice] = useState('')
  const [amount, setAmount] = useState('')
  const [type, setType] = useState('ask') // ask or bid

  useEffect(() => {
    // Check health
    api.getHealth().then(setHealth).catch(() => setHealth('Offline'))
    
    // Load products
    api.getProducts().then(prods => {
      setProducts(prods)
      if (prods.length > 0 && !selectedProduct) {
        setSelectedProduct(prods[0])
      }
    })
  }, [])

  useEffect(() => {
    if (selectedProduct) {
      loadOrders()
    }
  }, [selectedProduct])

  const loadOrders = () => {
    if (!selectedProduct) return
    api.getOrders(selectedProduct).then(orders => {
        // Sort by timestamp if possible, or leave as is
        setOrders(orders)
    })
  }

  const handleCreateOrder = async (e: React.FormEvent) => {
    e.preventDefault()
    try {
      await api.placeOrder({
        product: selectedProduct,
        price: parseFloat(price),
        amount: parseFloat(amount),
        type,
        username: "user1" // Hardcoded for now
      })
      alert("Order created!")
      loadOrders()
    } catch (err) {
      alert("Failed to create order")
    }
  }

  return (
    <div className="App">
      <header className="header">
        <h1>MiniATS Trading</h1>
        <span className={`status ${health === 'OK' ? 'online' : 'offline'}`}>
          Server: {health}
        </span>
      </header>
      
      <div className="main-layout">
        <aside className="sidebar">
          <h3>Products</h3>
          <ul>
            {products.map(p => (
              <li 
                key={p} 
                className={selectedProduct === p ? 'active' : ''}
                onClick={() => setSelectedProduct(p)}
              >
                {p}
              </li>
            ))}
          </ul>
        </aside>

        <main className="content">
          <h2>Order Book: {selectedProduct}</h2>
          
          <div className="order-form">
            <h3>Place Order</h3>
            <form onSubmit={handleCreateOrder}>
              <div className="form-group">
                <label>Type</label>
                <select value={type} onChange={e => setType(e.target.value)}>
                    <option value="ask">Ask (Sell)</option>
                    <option value="bid">Bid (Buy)</option>
                </select>
              </div>
              <div className="form-group">
                <label>Price</label>
                <input 
                    type="number" 
                    step="0.00000001"
                    value={price} 
                    onChange={e => setPrice(e.target.value)} 
                    required 
                />
              </div>
              <div className="form-group">
                <label>Amount</label>
                <input 
                    type="number" 
                    step="0.00000001"
                    value={amount} 
                    onChange={e => setAmount(e.target.value)} 
                    required 
                />
              </div>
              <button type="submit">Submit Order</button>
            </form>
          </div>

          <div className="orders-list">
            <h3>Active Orders</h3>
            <table>
                <thead>
                    <tr>
                        <th>Time</th>
                        <th>Type</th>
                        <th>Price</th>
                        <th>Amount</th>
                        <th>User</th>
                    </tr>
                </thead>
                <tbody>
                    {orders.map((o, i) => (
                        <tr key={i} className={o.type}>
                            <td>{o.timestamp}</td>
                            <td>{o.type.toUpperCase()}</td>
                            <td>{o.price.toFixed(8)}</td>
                            <td>{o.amount.toFixed(8)}</td>
                            <td>{o.username}</td>
                        </tr>
                    ))}
                </tbody>
            </table>
          </div>
        </main>
      </div>
    </div>
  )
}

export default App
