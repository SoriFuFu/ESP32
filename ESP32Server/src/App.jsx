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

    const handleGetConfig = () => {
        // const ws = new WebSocket('ws://192.168.1.222:82');
        // const ws = new WebSocket('ws://bubela.duckdns.org:82');
        const ws = new WebSocket('ws://' + window.location.hostname + ':82');

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
            
            ws.close(); // Cerrar la conexión después de recibir el mensaje
        };

        ws.onerror = (error) => {
            console.error('Error en WebSocket:', error);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket cerrada');
        };
    };

    useEffect(() => {
        handleGetConfig();
    }, []);

    
    const hundleSetWifiStatus = (status) => {
        setWifiStatus(status);
        console.log('hundleSetWifiStatus', status);
    };
    const hundleSetApStatus = (status) => {
        setApStatus(status);
        console.log('hundleSetApStatus', status);
    };
    const updateRelayApp = (relayName, statusOrName) => {
        if (typeof statusOrName === 'boolean') {
            // Actualizar estado del relé
            setRelay(prevConfig => ({
                ...prevConfig,
                [relayName]: {
                    ...prevConfig[relayName],
                    active: statusOrName
                }
            }));
        } else {
            // Actualizar nombre del relé
            setRelay(prevConfig => ({
                ...prevConfig,
                [relayName]: {
                    ...prevConfig[relayName],
                    name: statusOrName
                }
            }));
        }
    };
    

      
    const hundleSetEnable = (relay, status) => {
        updateRelayApp(relay, status);
    }
  
    const hundleSetName = (relay, name) => {
        updateRelayApp(relay, name);
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
                                <Route path="/config/*" element={<Config wifiConfig={wifiConfig} apConfig={apConfig} relay={relay} setWifiStatusApp={hundleSetWifiStatus} SetApStatusApp={hundleSetApStatus} setEnableApp={hundleSetEnable}  setNameApp={hundleSetName} />} />
                                
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

