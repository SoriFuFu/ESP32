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
    const [WifiConfig, setWifiConfig] = useState({});
    const [wifiEnabled, setWifiEnabled] = useState(false);
    const [apEnabled, setApEnabled] = useState(false);
    const [ApConfig, setApConfig] = useState({});
    const [Relay, setRelay] = useState({});
    useEffect(() => {
        const ws = new WebSocket('ws://192.168.0.101:81');
        // const ws = new WebSocket('ws://192.168.4.1:81');
        // const ws = new WebSocket('ws://' + window.location.hostname + ':81');
       
        ws.onopen = () => {
            console.log('Conexión WebSocket establecida');
            let message = { action: 'getConfig' };
            ws.send(JSON.stringify(message));
        };

        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            console.log('Mensaje recibido:', data);
            setWifiConfig(data.Wifi);
            setWifiEnabled(data.Wifi.status);
            setApEnabled(data.AP.status);
            setApConfig(data.AP);
            setRelay(data.Relay);
            setIsLoading(false);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket cerrada');
        };

        return () => {
            if (ws) {
                ws.close();
            }
        };
    }, []);

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
                    <Route path="/panel" element={<Panel Relay = {Relay}/>} />
                    <Route path="/config" element={<Config />} />
                    {/* Redirige a la última ruta visitada almacenada en localStorage */}
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
