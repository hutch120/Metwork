import React, { useEffect, useState } from 'react'
import logo from './logo.svg';
import './App.css';

// Note the proxy flag in package.json is how fetch works... TODO: test on the build version.
function App() {

  const [stations, setStations] = useState()

  useEffect(() => {
    async function getStations() {
      try {
        const res = await window.fetch('/stations') // Calls on port 5000 due to proxy flag in package.json
        setStations(await res.json())
      } catch (err) {
        console.log('Failed to get stations', err)
      }
    }
    getStations()
  }, [])

  if (!stations) {
    return null
  }

  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>
    </div>
  );
}

export default App;
