// Config.jsx
import React, { useState, useEffect } from 'react';
import { Routes, Route, Navigate } from 'react-router-dom';
import MenuConfig from './MenuConfig';
import WifiConfigComponent from './WifiConfig';
import ApConfig from './ApConfig';
import RelayConfig from './RelayConfig';
import Info from './Info';

const Config = ({wifiConfig, apConfig, relay, webSocket}) => {
    const [apStatus, setApStatus] = useState(apConfig.status);
    const [wifiStatus, setWifiStatus] = useState(wifiConfig.status);

    const hundleSetWifiStatus = (status) => {
        setWifiStatus(status);
    };
    const hundleSetApStatus = (status) => {
        setApStatus(status);
    };
    return (
        <div>
            <MenuConfig />

            <Routes>
                <Route path="/info" element={<Info wifiConfig={wifiConfig} apConfig={apConfig} relay={relay}  />} />
                <Route path="/wifi" element={<WifiConfigComponent wifiConfig={wifiConfig} apStatus={apStatus} setWifiStatusConfig={hundleSetWifiStatus} webSocket={webSocket}/>} />
                <Route path="/ap" element={<ApConfig apConfig={apConfig} wifiStatus={wifiStatus} setApStatusConfig={hundleSetApStatus} webSocket={webSocket}/>} />
                <Route path="/relay" element={<RelayConfig relay={relay} webSocket={webSocket}/>} />
                <Route path="*" element={<Navigate to="/config/info" replace />} />
            </Routes>
        </div>
    );
}

export default Config;
