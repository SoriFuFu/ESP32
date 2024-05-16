// Config.jsx
import React, { useState, useEffect } from 'react';
import { Routes, Route, Navigate } from 'react-router-dom';
import MenuConfig from './MenuConfig';
import WifiConfigComponent from './WifiConfig';
import ApConfig from './ApConfig';
import RelayConfig from './RelayConfig';
import Info from './Info';
import Reset from './Reset';

const Config = ({ wifiConfig, apConfig, relay, setWifiStatusApp, SetApStatusApp, setEnableApp,  setNameApp }) => {
    const [apStatus, setApStatus] = useState(apConfig.status);
    const [wifiStatus, setWifiStatus] = useState(wifiConfig.status);
    const [relayConfig, setRelayConfig] = useState(relay);
    const [webSocket, setWebSocket] = useState(null);
    
    useEffect(() => {
        handleGetConfig();
    }, []);

    const handleGetConfig = () => {
        // const ws = new WebSocket('ws://192.168.1.222:81');
        const ws = new WebSocket('ws://bubela.duckdns.org:81');
        // const ws = new WebSocket('ws://' + window.location.hostname + ':81');
        setWebSocket(ws);
        
        ws.onerror = (error) => {
            console.error('Error en WebSocket:', error);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket cerrada');
        };
    };
    
    const hundleSetWifiStatus = (status) => {
        setWifiStatus(status);
        setWifiStatusApp(status);
    };
    const hundleSetApStatus = (status) => {
        setApStatus(status);
        SetApStatusApp(status);
    };

    const updateRelayConfig = (relayName, statusOrName) => {
        if (typeof statusOrName === 'boolean') {
            // Actualizar estado del relé
            setRelayConfig(prevConfig => ({
                ...prevConfig,
                [relayName]: {
                    ...prevConfig[relayName],
                    active: statusOrName
                }
            }));
        } else {
            // Actualizar nombre del relé
            setRelayConfig(prevConfig => ({
                ...prevConfig,
                [relayName]: {
                    ...prevConfig[relayName],
                    name: statusOrName
                }
            }));
        }
    };
    
    const hundleSetEnable = (relay, status) => {
        updateRelayConfig(relay, status);
        setEnableApp(relay, status);
        console.log(relayConfig);
    }
    
    const hundleSetName = (relay, name) => {
        updateRelayConfig(relay, name);
        setNameApp(relay, name);
        
    }
    

    return (
        <div>
            <MenuConfig />

            <Routes>
                <Route path="/info" element={<Info wifiConfig={wifiConfig} apConfig={apConfig} relay={relayConfig} />} />
                <Route path="/wifi" element={<WifiConfigComponent wifiConfig={wifiConfig} apStatus={apStatus} setWifiStatusConfig={hundleSetWifiStatus} webSocket={webSocket} />} />
                <Route path="/ap" element={<ApConfig apConfig={apConfig} wifiStatus={wifiStatus} setApStatusConfig={hundleSetApStatus} webSocket={webSocket} />} />
                <Route path="/relay" element={<RelayConfig relay={relayConfig} setEnabledConfig={hundleSetEnable}  setNameConfig={hundleSetName} webSocket={webSocket} />} />
                <Route path="/reset" element={<Reset webSocket={webSocket} />} />
                <Route path="*" element={<Navigate to="/config/info" replace />} />
            </Routes>
        </div>
    );
}

export default Config;
