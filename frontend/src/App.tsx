import { useState, useEffect } from 'react'
import './App.css'
import { api } from './api'
import type { Order } from './api'

type View = 'market' | 'orders' | 'wallet'
type AuthState = 'login' | 'register' | 'loggedin'

function App() {
  const [authState, setAuthState] = useState<AuthState>('login')
  const [currentUser, setCurrentUser] = useState('')
  const [currentView, setCurrentView] = useState<View>('market')
  
  // Login/Register Form
  const [username, setUsername] = useState('')
  const [password, setPassword] = useState('')

  // Data
  const [products, setProducts] = useState<string[]>([])
  const [marketOrders, setMarketOrders] = useState<Order[]>([])
  const [userOrders, setUserOrders] = useState<Order[]>([])
  const [wallet, setWallet] = useState<any>(null)
  const [health, setHealth] = useState<string>('Unknown')
  
  // Market View State
  const [selectedProduct, setSelectedProduct] = useState('')
  const [price, setPrice] = useState('')
  const [amount, setAmount] = useState('')
  const [type, setType] = useState('ask')

  // Wallet Actions
  const [depCur, setDepCur] = useState('USDT')
  const [depAmt, setDepAmt] = useState('')
  const [wdCur, setWdCur] = useState('USDT')
  const [wdAmt, setWdAmt] = useState('')

  // New Product
  const [newProduct, setNewProduct] = useState('')

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

  // Auto-refresh data
  useEffect(() => {
    if (authState !== 'loggedin') return;

    const refresh = () => {
        if (currentView === 'market' && selectedProduct) {
            loadMarketOrders()
        }
        if (currentView === 'orders') {
            loadUserOrders()
        }
        if (currentView === 'wallet') {
            loadWallet()
        }
    }
    
    refresh();
    const interval = setInterval(refresh, 2000);
    return () => clearInterval(interval);
  }, [authState, currentView, selectedProduct])


  const loadMarketOrders = () => {
    if (!selectedProduct) return
    api.getOrders(selectedProduct).then(orders => {
        setMarketOrders(orders || [])
    }).catch(err => {
        console.error("Failed to load orders", err);
        setMarketOrders([])
    })
  }

  const loadUserOrders = () => {
      api.getUserOrders(currentUser).then(setUserOrders)
  }

  const loadWallet = () => {
      api.getWallet(currentUser).then(setWallet)
  }

  const handleLogin = async (e: React.FormEvent) => {
      e.preventDefault()
      try {
          await api.login({username, password})
          setCurrentUser(username)
          setAuthState('loggedin')
          alert("Welcome back!")
      } catch (e) {
          alert("Login failed")
      }
  }

  const handleRegister = async (e: React.FormEvent) => {
      e.preventDefault()
      try {
          await api.register({username, password})
          alert("Registered! Please login.")
          setAuthState('login')
      } catch (e) {
          alert("Registration failed")
      }
  }

  const handleCreateOrder = async (e: React.FormEvent) => {
    e.preventDefault()
    try {
      await api.placeOrder({
        product: selectedProduct,
        price: parseFloat(price),
        amount: parseFloat(amount),
        type,
        username: currentUser
      })
      alert("Order created!")
      loadMarketOrders()
      setPrice('')
      setAmount('')
    } catch (err) {
      alert("Failed to create order (Insufficient funds?)")
    }
  }
  
  const handleCancelOrder = async (id: number) => {
      try {
          await api.cancelOrder(id, currentUser)
          loadUserOrders()
      } catch (e) {
          alert("Failed to cancel order")
      }
  }

  const handleDeposit = async (e: React.FormEvent) => {
      e.preventDefault()
      try {
          await api.deposit({username: currentUser, product: depCur, amount: parseFloat(depAmt)})
          alert("Deposit successful")
          loadWallet()
          setDepAmt('')
      } catch (e) {
          alert("Deposit failed")
      }
  }

  const handleWithdraw = async (e: React.FormEvent) => {
      e.preventDefault()
      try {
          await api.withdraw({username: currentUser, product: wdCur, amount: parseFloat(wdAmt)})
          alert("Withdraw successful")
          loadWallet()
          setWdAmt('')
      } catch (e) {
          alert("Withdraw failed (Insufficient funds?)")
      }
  }

  const handleAddProduct = (e: React.FormEvent) => {
      e.preventDefault()
      const p = newProduct.trim().toUpperCase()
      if (p && !products.includes(p)) {
          // Temporarily add to list (backend will confirm it once order is placed)
          setProducts(prev => [p, ...prev])
          setSelectedProduct(p)
          setNewProduct('')
      }
  }

  if (authState === 'login' || authState === 'register') {
      return (
          <div className="auth-container">
              <h1>MiniATS Exchange</h1>
              <div className="auth-box">
                <h2>{authState === 'login' ? 'Login' : 'Register'}</h2>
                <form onSubmit={authState === 'login' ? handleLogin : handleRegister}>
                    <div className="form-group">
                        <label>Username</label>
                        <input value={username} onChange={e => setUsername(e.target.value)} required />
                    </div>
                    <div className="form-group">
                        <label>Password</label>
                        <input type="password" value={password} onChange={e => setPassword(e.target.value)} required />
                    </div>
                    <button type="submit">{authState === 'login' ? 'Login' : 'Register'}</button>
                </form>
                <p onClick={() => setAuthState(authState === 'login' ? 'register' : 'login')} className="link">
                    {authState === 'login' ? 'Need an account? Register' : 'Have an account? Login'}
                </p>
              </div>
          </div>
      )
  }

  return (
    <div className="App">
      <header className="header">
        <div className="header-content">
            <div className="logo">MiniATS <span>PRO</span></div>
            <nav>
                <button className={currentView === 'market' ? 'active':''} onClick={() => setCurrentView('market')}>Market</button>
                <button className={currentView === 'orders' ? 'active':''} onClick={() => setCurrentView('orders')}>My Orders</button>
                <button className={currentView === 'wallet' ? 'active':''} onClick={() => setCurrentView('wallet')}>Wallet</button>
            </nav>
            <div className="user-info">
                <span>{currentUser}</span>
                <button className="logout-btn" onClick={() => setAuthState('login')}>Logout</button>
            </div>
        </div>
      </header>
      
      <div className="main-layout">
        {currentView === 'market' && (
            <>
                <aside className="sidebar">
                <h3>Markets</h3>
                <div className="add-product-box">
                    <form onSubmit={handleAddProduct}>
                         <input 
                            placeholder="+ Pair (e.g. BTC/USD)" 
                            value={newProduct}
                            onChange={e => setNewProduct(e.target.value)}
                        />
                        <button type="submit">+</button>
                    </form>
                </div>
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
                <div className="market-header">
                    <h2>{selectedProduct} Order Book</h2>
                </div>
                
                <div className="order-book-container">
                    <div className="bids-column">
                        <h3>Bids (Buy)</h3>
                        <table>
                            <thead><tr><th>Price</th><th>Amount</th><th>User</th></tr></thead>
                            <tbody>
                                {marketOrders.filter(o => o.type === 'bid').sort((a,b) => b.price - a.price).map(o => (
                                    <tr key={o.id}>
                                        <td className="price-up">{o.price.toFixed(5)}</td>
                                        <td>{o.amount.toFixed(8)}</td>
                                        <td>{o.username}</td>
                                    </tr>
                                ))}
                            </tbody>
                        </table>
                    </div>
                    <div className="asks-column">
                        <h3>Asks (Sell)</h3>
                         <table>
                            <thead><tr><th>Price</th><th>Amount</th><th>User</th></tr></thead>
                            <tbody>
                                {marketOrders.filter(o => o.type === 'ask').sort((a,b) => a.price - b.price).map(o => (
                                    <tr key={o.id}>
                                        <td className="price-down">{o.price.toFixed(5)}</td>
                                        <td>{o.amount.toFixed(8)}</td>
                                        <td>{o.username}</td>
                                    </tr>
                                ))}
                            </tbody>
                        </table>
                    </div>
                </div>

                <div className="order-form-panel">
                    <h3>Place Limit Order</h3>
                    <form onSubmit={handleCreateOrder}>
                        <div className="form-row">
                            <div className="form-group">
                                <label>Side</label>
                                <div className="toggle-group">
                                    <button type="button" className={type === 'bid' ? 'active-bid':''} onClick={()=>setType('bid')}>Buy</button>
                                    <button type="button" className={type === 'ask' ? 'active-ask':''} onClick={()=>setType('ask')}>Sell</button>
                                </div>
                            </div>
                            <div className="form-group">
                                <label>Price</label>
                                <input type="number" step="0.00000001" value={price} onChange={e => setPrice(e.target.value)} required />
                            </div>
                            <div className="form-group">
                                <label>Amount</label>
                                <input type="number" step="0.00000001" value={amount} onChange={e => setAmount(e.target.value)} required />
                            </div>
                            <div className="form-group">
                                <label>&nbsp;</label>
                                <button type="submit" className={type === 'bid' ? 'btn-buy':'btn-sell'}>
                                    {type === 'bid' ? 'Buy' : 'Sell'} {selectedProduct}
                                </button>
                            </div>
                        </div>
                    </form>
                </div>
                </main>
            </>
        )}

        {currentView === 'orders' && (
            <main className="content full-width">
                <h2>My Open Orders</h2>
                <table className="data-table">
                    <thead>
                        <tr>
                            <th>Time</th>
                            <th>Pair</th>
                            <th>Type</th>
                            <th>Price</th>
                            <th>Amount</th>
                            <th>Action</th>
                        </tr>
                    </thead>
                    <tbody>
                        {userOrders.map(o => (
                            <tr key={o.id}>
                                <td>{o.timestamp}</td>
                                <td>{o.product}</td>
                                <td className={o.type === 'bid' ? 'text-green':'text-red'}>{o.type.toUpperCase()}</td>
                                <td>{o.price}</td>
                                <td>{o.amount}</td>
                                <td><button onClick={()=>handleCancelOrder(o.id)}>Cancel</button></td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </main>
        )}

        {currentView === 'wallet' && wallet && (
             <main className="content full-width">
                 <h2>Wallet Management</h2>
                 <div className="wallet-grid">
                     <div className="wallet-card">
                         <h3>Available Balances</h3>
                         <ul>
                             {Object.entries(wallet.currencies || {}).map(([cur, amt]: [string, any]) => (
                                 <li key={cur}>
                                     <span className="coin">{cur}</span>
                                     <span className="val">{parseFloat(amt).toFixed(8)}</span>
                                 </li>
                             ))}
                         </ul>
                         <h3>Locked in Orders</h3>
                         <ul>
                             {Object.entries(wallet.locked || {}).map(([cur, amt]: [string, any]) => (
                                 <li key={cur}>
                                     <span className="coin">{cur}</span>
                                     <span className="val">{parseFloat(amt).toFixed(8)}</span>
                                 </li>
                             ))}
                         </ul>
                     </div>
                     <div className="wallet-card">
                        <h3>Transfer Funds</h3>
                        <div className="dw-forms">
                            <div className="dw-form">
                                <h4>Deposit</h4>
                                <form onSubmit={handleDeposit}>
                                    <input placeholder="Currency" value={depCur} onChange={e=>setDepCur(e.target.value)} required />
                                    <input type="number" step="0.00000001" placeholder="Amount" value={depAmt} onChange={e=>setDepAmt(e.target.value)} required />
                                    <button type="submit" className="btn-deposit">Deposit</button>
                                </form>
                            </div>
                             <div className="dw-form">
                                <h4>Withdraw</h4>
                                <form onSubmit={handleWithdraw}>
                                    <input placeholder="Currency" value={wdCur} onChange={e=>setWdCur(e.target.value)} required />
                                    <input type="number" step="0.00000001" placeholder="Amount" value={wdAmt} onChange={e=>setWdAmt(e.target.value)} required />
                                    <button type="submit" className="btn-withdraw">Withdraw</button>
                                </form>
                            </div>
                        </div>
                     </div>
                 </div>
             </main>
        )}
      </div>
    
      <footer className="footer-status">
          System Status: <span className={health === 'OK' ? 'status-ok' : 'status-bad'}>{health}</span>
      </footer>
    </div>
  )
}

export default App