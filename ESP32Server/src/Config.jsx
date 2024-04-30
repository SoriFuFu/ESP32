// Config.jsx
import React, { useState, useEffect } from 'react';
import { Routes, Route, Navigate } from 'react-router-dom';
import MenuConfig from './MenuConfig';
import WifiConfigComponent from './WifiConfig';
import ApConfig from './ApConfig';
import RelayConfig from './RelayConfig';
import Info from './Info';
import Reset from './Reset';

const Config = ({ wifiConfig, apConfig, relay, webSocket, setWifiStatusApp, SetApStatusApp, setK1EnableApp, setK2EnableApp, setK3EnableApp, setK4EnableApp }) => {
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

    const updateRelayConfig = (relayName, status) => {
        console.log(relayName, status);
        setRelayConfig(prevConfig => ({
          ...prevConfig,
          [relayName]: {
            ...prevConfig[relayName],
            active: status
          }
        }));
        
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

    return (
        <div>
            <MenuConfig />

            <Routes>
                <Route path="/info" element={<Info wifiConfig={wifiConfig} apConfig={apConfig} relay={relayConfig} />} />
                <Route path="/wifi" element={<WifiConfigComponent wifiConfig={wifiConfig} apStatus={apStatus} setWifiStatusConfig={hundleSetWifiStatus} webSocket={webSocket} />} />
                <Route path="/ap" element={<ApConfig apConfig={apConfig} wifiStatus={wifiStatus} setApStatusConfig={hundleSetApStatus} webSocket={webSocket} />} />
                <Route path="/relay" element={<RelayConfig relay={relayConfig} setK1EnabledConfig={hundleSetK1Enable} setK2EnabledConfig={hundleSetK2Enable} setK3EnabledConfig={hundleSetK3Enable} setK4EnabledConfig={hundleSetK4Enable} webSocket={webSocket} />} />
                <Route path="/reset" element={<Reset />} />
                <Route path="*" element={<Navigate to="/config/info" replace />} />
            </Routes>
        </div>
    );
}

export default Config;
