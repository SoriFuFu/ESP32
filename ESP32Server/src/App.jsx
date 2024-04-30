// App.jsx
import React, { useEffect, useState } from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';
import { Container, Spinner } from 'react-bootstrap';
import { BrowserRouter as Router, Routes, Route, Navigate } from 'react-router-dom';
import './App.css';
import Menu from './Menu';
import Panel from './Panel';
import Config from './Config';

const App = () => {
    const [isLoading, setIsLoading] = useState(true);
    const [wifiConfig, setWifiConfig] = useState({});
    const [wifiStatus, setWifiStatus] = useState(false);
    const [apStatus, setApStatus] = useState(false);
    const [apConfig, setApConfig] = useState({});
    const [relay, setRelay] = useState({});
    const [webSocket, setWebSocket] = useState(null);
    

    useEffect(() => {
        handleGetConfig();
    }, []);

    const handleGetConfig = () => {
        const ws = new WebSocket('ws://192.168.1.38:81');
        // const ws = new WebSocket('ws://192.168.4.1:81');
        // const ws = new WebSocket('ws://' + window.location.hostname + ':81');
        setWebSocket(ws);
        
        ws.onopen = () => {
            let message = { action: 'getConfig' };
            ws.send(JSON.stringify(message));
        };
        
        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            setWifiConfig(data.Wifi);
            setWifiStatus(data.Wifi.status);
            setApStatus(data.AP.status);
            setApConfig(data.AP);
            setRelay(data.Relay);
            setIsLoading(false);
        };

        ws.onerror = (error) => {
            console.error('Error en WebSocket:', error);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket cerrada');
        };
    };
    const hundleSetWifiStatus = (status) => {
        setWifiStatus(status);
        console.log('hundleSetWifiStatus', status);
    };
    const hundleSetApStatus = (status) => {
        setApStatus(status);
        console.log('hundleSetApStatus', status);
    };
    const updateRelayApp = (relayName, status) => {
        console.log(relayName, status);
        setRelay(prevConfig => ({
          ...prevConfig,
          [relayName]: {
            ...prevConfig[relayName],
            active: status
          }
        }));
        
      };

      
    const hundleSetK1Enable = (status) => {
        updateRelayApp('K1', status);
    }
    const hundleSetK2Enable = (status) => {
        updateRelayApp('K2', status);
    }
    const hundleSetK3Enable = (status) => {
        updateRelayApp('K3', status);
    }
    const hundleSetK4Enable = (status) => {
        updateRelayApp('K4', status);
    }


    return (
        <Router>
            <div>
                {/* Elemento de carga condicional */}
                {isLoading ? (
                    <div className="d-flex justify-content-center align-items-center" style={{ minHeight: '100vh' }}>
                        <Spinner animation="border" role="status">
                            <span className="visually-hidden">Cargando...</span>
                        </Spinner>
                    </div>
                ) : (
                    <>
                        <Menu wifiStatus={wifiStatus} apStatus={apStatus} />
                        <Container fluid className="content">
                            <Routes>
                                <Route path="/panel" element={<Panel Relay={relay} />} />
                                <Route path="/config/*" element={<Config wifiConfig={wifiConfig} apConfig={apConfig} relay={relay} webSocket={webSocket} setWifiStatusApp={hundleSetWifiStatus} SetApStatusApp={hundleSetApStatus} setK1EnableApp={hundleSetK1Enable} setK2EnableApp={hundleSetK2Enable} setK3EnableApp={hundleSetK3Enable} setK4EnableApp={hundleSetK4Enable} />} />
                                
                                <Route path="*" element={<Navigate to="/panel" replace />} />
                            </Routes>
                        </Container>
                    </>
                )}
            </div>
        </Router>
    );
}

export default App;

