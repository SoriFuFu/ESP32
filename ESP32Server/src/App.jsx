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
    const [wifiEnabled, setWifiEnabled] = useState(false);
    const [apEnabled, setApEnabled] = useState(false);
    const [apConfig, setApConfig] = useState({});
    const [relay, setRelay] = useState({});
    const [webSocket, setWebSocket] = useState(null);
    

    useEffect(() => {
        handleGetConfig();
    }, []);

    const handleGetConfig = () => {
        const ws = new WebSocket('ws://192.168.1.40:81');
        setWebSocket(ws);
        
        ws.onopen = () => {
            let message = { action: 'getConfig' };
            ws.send(JSON.stringify(message));
        };
        
        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            setWifiConfig(data.Wifi);
            setWifiEnabled(data.Wifi.status);
            setApEnabled(data.AP.status);
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
                        <Menu wifiEnabled={wifiEnabled} apEnabled={apEnabled} />
                        <Container fluid className="content">
                            <Routes>
                                <Route path="/panel" element={<Panel Relay={relay} />} />
                                <Route path="/config/*" element={<Config wifiConfig={wifiConfig} apConfig={apConfig} relay={relay} webSocket={webSocket} />} />
                                
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

