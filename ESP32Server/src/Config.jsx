// Config.jsx
import React, { useState, useEffect } from 'react';
import { Routes, Route, Navigate } from 'react-router-dom';
import MenuConfig from './MenuConfig';
import WifiConfigComponent from './WifiConfig';
import ApConfig from './ApConfig';
import RelayConfig from './RelayConfig';
import Info from './Info';
import Reset from './Reset';
import Logs from './Logs';

const Config = ({ webSocket, deviceInfo, wifiConfig, wifiNetworks, apConfig, relayConfig, logs, updateWifiConfigApp, updateApConfigApp, updateRelayApp }) => {
    const [wifiStatus, setWifiStatus] = useState(wifiConfig.status);
    const [apStatus, setApStatus] = useState(apConfig.status);

    const updateWifiConfig = (object, data) => {
        updateWifiConfigApp(object, data);
    };
    const updateApConfig = (object, data) => {
        updateApConfigApp(object, data);
    };
    const updateRelay = (relayName, object, data) => {
        updateRelayApp(relayName, object, data);
    };
    

    return (
        <div>
            <MenuConfig />

            <Routes>
                <Route path="/info" element={<Info deviceInfo={deviceInfo} wifiConfig={wifiConfig} apConfig={apConfig} relay={relayConfig} />} />
                <Route path="/wifi" element={<WifiConfigComponent wifiConfig={wifiConfig} wifiNetworks={wifiNetworks} apStatus={apStatus} updateWifiConfig={updateWifiConfig} webSocket={webSocket} />} />
                <Route path="/ap" element={<ApConfig apConfig={apConfig} wifiStatus={wifiStatus} updateApConfig={updateApConfig} webSocket={webSocket} />} />
                <Route path="/relay" element={<RelayConfig relay={relayConfig} updateRelay={updateRelay} webSocket={webSocket} />} />
                <Route path="/reset" element={<Reset webSocket={webSocket} />} />
                <Route path="/log" element={<Logs webSocket={webSocket} logs={logs}/>} />
                <Route path="*" element={<Navigate to="/config/info" replace />} />
            </Routes>
        </div>
    );
}

export default Config;
