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
    const [apConfig, setApConfig] = useState({});
    const [relayConfig, setRelayConfig] = useState({});
    const [deviceInfo, setDeviceInfo] = useState({});
    const [logs, setLogs] = useState([]);
    const [wifiNetworks, setWifiNetworks] = useState([]);
    const [webSocket, setWebSocket] = useState(null);

    const handleGetConfig = () => {

        // const ws = new WebSocket('ws://bubela.duckdns.org/ws');
        const ws = new WebSocket('ws://' + window.location.hostname + '/ws');


        setWebSocket(ws);
        ws.onopen = () => {
            const now = new Date();
            const hour = now.getHours();
            const minute = now.getMinutes();
            const seconds = now.getSeconds();
            const day = now.getDate(); // Cambiado de getDay() a getDate()
            const month = now.getMonth() + 1; // getMonth() devuelve de 0 a 11, sumamos 1
            const year = now.getFullYear();
            let message = { action: 'GETCONFIG', hour: hour, minute: minute, seconds: seconds, day: day, month: month, year: year };
            ws.send(JSON.stringify(message));
        };
        
        ws.onmessage = (event) => {
            const message = JSON.parse(event.data);
            const data = message.Data;
            const action = message.action;
            if (action === 'GETLOGS') {
                setLogs(data.logs);
                return;
            } else if (action === 'GETCONFIG') {
                setWifiConfig(data.Wifi);
                setApConfig(data.AP);
                setRelayConfig(data.Relay);
                setDeviceInfo(data.Device);
                setIsLoading(false);
            } else if (action === 'GETNETWORKS') {
                setWifiNetworks(message.networks);
             
            }else if (action === 'UPDATE_TIMER') {
                let K1Time = message.timeK1;
                let K2Time = message.timeK2;
                updateRelayApp('K1', "remainingTime", K1Time);
                updateRelayApp('K2', "remainingTime", K2Time);



            }else if (action === 'UPDATE_RELAY_STATUS') {
                let relayName = message.relay;
                let relayStatus = message.command;
                updateRelayApp(relayName, "state", relayStatus);
        
            }else if (action === 'MESSAGE') {
                if (message.command === 'SUCCESS') {
                    showSuccessAlert(message.message);
                } else if (message.command === 'ERROR') {
                    showErrorAlert(message.message);
                }
            }
        };

        ws.onerror = (error) => {
            console.error('Error en WebSocket:', error);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket Data cerrada');
        };
    };

    useEffect(() => {
        handleGetConfig();
    }, []);


    const updateWifiConfigApp= (object, data) => {
        setWifiConfig(prevConfig => ({
            ...prevConfig,
            [object]: data
        }));
    };
    const updateApConfigApp = (object, data) => {
        setApConfig(prevConfig => ({
            ...prevConfig,
            [object]: data
        }));
    };

    const updateRelayApp = (relayName, object, data) => {
        setRelayConfig(prevConfig => ({
            ...prevConfig,
            [relayName]: {
                ...prevConfig[relayName],
                [object]: data
            }
        }));
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
                        <Menu wifiStatus={wifiConfig.status} apStatus={apConfig.status} />
                        <Container fluid className="content">
                            <Routes>
                                <Route path="/panel" element={<Panel webSocket={webSocket} Relay={relayConfig} updateRelayApp={updateRelayApp} />} />
                                <Route path="/config/*" element={<Config webSocket={webSocket} deviceInfo={deviceInfo} wifiConfig={wifiConfig} wifiNetworks={wifiNetworks} apConfig={apConfig} relayConfig={relayConfig} logs={logs} updateWifiConfigApp={updateWifiConfigApp}  updateApConfigApp={updateApConfigApp} updateRelayApp={updateRelayApp} />} />

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

