// Config.jsx
import React, { useState, useEffect } from 'react';
import { Routes, Route, Navigate } from 'react-router-dom';
import MenuConfig from './MenuConfig';
import WifiConfigComponent from './WifiConfig';
import ApConfig from './ApConfig';
import RelayConfig from './RelayConfig';
import Info from './Info';
import Reset from './Reset';

const Config = ({ wifiConfig, apConfig, relay, webSocket, setWifiStatusApp, SetApStatusApp, setK1EnableApp, setK2EnableApp, setK3EnableApp, setK4EnableApp, setK1NameApp, setK2NameApp,  setK3NameApp, setK4NameApp, }) => {
    const [apStatus, setApStatus] = useState(apConfig.status);
    const [wifiStatus, setWifiStatus] = useState(wifiConfig.status);
    const [relayConfig, setRelayConfig] = useState(relay);
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
    

      
    const hundleSetK1Enable = (status) => {
        updateRelayConfig('K1', status);
        setK1EnableApp(status);
        console.log(relayConfig);
    }
    const hundleSetK2Enable = (status) => {
        updateRelayConfig('K2', status);
        setK2EnableApp(status);
        console.log(relayConfig);
    }
    const hundleSetK3Enable = (status) => {
        updateRelayConfig('K3', status);
        setK3EnableApp(status);
        console.log(relayConfig);
    }
    const hundleSetK4Enable = (status) => {
        updateRelayConfig('K4', status);
        setK4EnableApp(status);
        console.log(relayConfig);
    }
    const hundleSetK1Name = (name) => {
        updateRelayConfig('K1', name);
        setK1NameApp(name);
        
    }
    const hundleSetK2Name = (name) => {
        updateRelayConfig('K2', name);
        setK2NameApp(name);
    }
    const hundleSetK3Name = (name) => {
        updateRelayConfig('K3', name);
        setK3NameApp(name);
    }
    const hundleSetK4Name = (name) => {
        updateRelayConfig('K4', name);
        setK4NameApp(name);
    }

    return (
        <div>
            <MenuConfig />

            <Routes>
                <Route path="/info" element={<Info wifiConfig={wifiConfig} apConfig={apConfig} relay={relayConfig} />} />
                <Route path="/wifi" element={<WifiConfigComponent wifiConfig={wifiConfig} apStatus={apStatus} setWifiStatusConfig={hundleSetWifiStatus} webSocket={webSocket} />} />
                <Route path="/ap" element={<ApConfig apConfig={apConfig} wifiStatus={wifiStatus} setApStatusConfig={hundleSetApStatus} webSocket={webSocket} />} />
                <Route path="/relay" element={<RelayConfig relay={relayConfig} setK1EnabledConfig={hundleSetK1Enable} setK2EnabledConfig={hundleSetK2Enable} setK3EnabledConfig={hundleSetK3Enable} setK4EnabledConfig={hundleSetK4Enable} setK1NameConfig={hundleSetK1Name} setK2NameConfig={hundleSetK2Name} setK3NameConfig={hundleSetK3Name} setK4NameConfig={hundleSetK4Name} webSocket={webSocket} />} />
                <Route path="/reset" element={<Reset />} />
                <Route path="*" element={<Navigate to="/config/info" replace />} />
            </Routes>
        </div>
    );
}

export default Config;
