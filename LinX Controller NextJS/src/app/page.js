// File: src/app/page.js

'use client';
import { useState, useEffect } from 'react';

export default function RobotControl() {
  const [movement, setMovement] = useState('Stop');
  const [ledState, setLedState] = useState(false);
  const [ssid, setSSID] = useState('LinX');
  const [password, setPassword] = useState('linxrobot');
  const [isLoading, setIsLoading] = useState(true); // Loading state for initial fetch

  useEffect(() => {
    const fetchInitialState = async () => {
      try {
        const ledResponse = await fetch('http://192.168.4.1/ledState');
        if (ledResponse.ok) {
          const data = await ledResponse.json();
          setLedState(data.state === 'ON');
        }
      } catch (error) {
        console.error('Error fetching initial LED state:', error);
      } finally {
        setIsLoading(false);
      }
    };

    fetchInitialState();
  }, []);

  const sendCommand = async (command) => {
    try {
      const response = await fetch(`http://192.168.4.1/?direction=${command}`, { method: 'GET' });
      if (!response.ok) {
        throw new Error('Network response was not ok.');
      }
      setMovement(command);
    } catch (error) {
      console.error('Error sending command:', error);
    }
  };

  const toggleLED = async () => {
    try {
      setIsLoading(true);
      const response = await fetch('http://192.168.4.1/toggleLED', { method: 'POST' });
      if (!response.ok) {
        throw new Error('Network response was not ok.');
      } else {
        setLedState(!ledState);
      }
    } catch (error) {
      console.error('Error toggling LED:', error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
      <div className="container">
        <h1>Robot Control</h1>

        {/* Display loading message while fetching initial LED state */}
        {isLoading ? (
            <p className="status">Loading...</p>
        ) : (
            <>
              <p className="status">Current Movement: {movement}</p>
              <p className="status">LED State: {ledState ? 'OFF' : 'ON'}</p>
            </>
        )}
        <p><b>Connect to Wi-Fi:</b> {ssid}</p>
        <p><b>Password:</b> {password}</p>

        <div className="button-grid">
          <button onClick={() => sendCommand('forward')}>&#9650;</button>
          <div>
            <button onClick={() => sendCommand('left')}>&#9668;</button>
            <button onClick={() => sendCommand('stop')}>&#9632;</button>
            <button onClick={() => sendCommand('right')}>&#9658;</button>
          </div>
          <button onClick={() => sendCommand('backward')}>&#9660;</button>
        </div>

        <div className="led-control">
          <button className="toggle-led" onClick={toggleLED} disabled={isLoading}>
            {isLoading ? 'Toggling...' : 'Toggle LED'}
          </button>
        </div>

        <style jsx>{`
        .container {
          font-family: sans-serif;
          text-align: center;
          padding: 20px;
          background: linear-gradient(to bottom right, #4CAF50, #2E8B57); 
          color: white;
          border-radius: 10px;
          box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); 
        }

        h1 {
          margin-bottom: 20px;
        }

        .status {
          font-size: 20px;
          margin-bottom: 10px;
        }

        .button-grid {
          display: grid;
          grid-template-columns: 1fr 2fr 1fr;
          gap: 10px;
        }

        button {
          background-color: #2E8B57; 
          color: white;
          padding: 15px 20px;
          border: none;
          border-radius: 5px;
          font-size: 18px;
          cursor: pointer;
          transition: background-color 0.3s;
        }

        button:hover {
          background-color: #1E6941; 
        }

        .led-control {
          margin-top: 20px;
        }

        .toggle-led {
          width: 150px; 
        }
      `}</style>
      </div>
  );
}
