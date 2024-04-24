// Config.jsx
import React from 'react';
import { Routes, Route, Navigate } from 'react-router-dom';
import MenuConfig from './MenuConfig';
import WifiConfig from './WifiConfig';
import ApConfig from './ApConfig';
import RelayConfig from './RelayConfig';
import Info from './Info';

const Config = ({wifiConfig, apConfig, relay}) => {
    return (
        <div>
            <MenuConfig />

            <Routes>
                <Route path="/info" element={<Info wifiConfig={wifiConfig} apConfig={apConfig} relay={relay}  />} />
                <Route path="/wifi" element={<WifiConfig />} />
                <Route path="/ap" element={<ApConfig />} />
                <Route path="/relay" element={<RelayConfig />} />
                <Route path="*" element={<Navigate to="/config/info" replace />} />
            </Routes>
        </div>
    );
}

export default Config;
