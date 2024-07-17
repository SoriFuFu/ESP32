import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';


const WifiConfigComponent = ({ wifiConfig, wifiNetworks, apStatus, updateWifiConfig, webSocket }) => {

    // ESTADOS PARA LA CONEXIÓN DE RED
    const [isLoadingSearchNetworks, setIsLoadingSearchNetworks] = useState(false); // Estado para mostrar el spinner de búsqueda de redes
    const [isLoadingSetWifiConfig, setIsLoadingSetWifiConfig] = useState(false);   // Estado para mostrar el spinner de configuración de WiFi
    const [showWifiPassword, setShowWifiPassword] = useState(false);               // Estado para mostrar la contraseña de la red WiFi
    const [selectedNetwork, setSelectedNetwork] = useState(wifiConfig.ssid);      // Estado para almacenar la red WiFi seleccionada      

    useEffect(() => {
        if (wifiNetworks.length >= 0) {
            setIsLoadingSearchNetworks(false);
        }
    }, [wifiNetworks]);

    // OBTENER LAS REDES DISPONIBLES 
    const handleGetNetworks = () => {
        if (webSocket) {
            setIsLoadingSearchNetworks(true);
            let message = { action: 'GETNETWORKS' };
            webSocket.send(JSON.stringify(message));
        } else {
            console.log('WebSocket no está inicializado');
        }
    };
    //CONFIGURAR WIFI
    const setConfigWifi = () => {
        setIsLoadingSetWifiConfig(true);
        if (selectedNetwork === '' || wifiConfig.password === '' || wifiConfig.ip === '' || wifiConfig.subnet === '' || wifiConfig.gateway === '') {
            showErrorAlert('Debes llenar todos los campos');
            setIsLoadingSetWifiConfig(false);
            return;
        }
        if (webSocket) {
            let message = { action: 'SETWIFICONFIG', wifiActive: true, ssid: selectedNetwork, password: wifiConfig.password, ip: wifiConfig.ip, subnet: wifiConfig.subnet, gateway: wifiConfig.gateway };
            webSocket.send(JSON.stringify(message));
            updateWifiConfig("active", true);
            updateWifiConfig("status", true);
            updateWifiConfig("ssid", selectedNetwork);
        } else {
            console.log('WebSocket no está inicializado');
        }
    };
    //ACTIVAR O DESACTIVAR LA RED WIFI
    const handleWifiEnabledChange = (checked) => {
        if (apStatus && !checked) {
            if (wifiConfig.ssid !== '') {
            updateWifiConfig("active", false);
            updateWifiConfig("status", false);
            updateWifiConfig("ssid", '');
            updateWifiConfig("password", '');
            updateWifiConfig("ip", '');
            updateWifiConfig("subnet", '');
            updateWifiConfig("gateway", '');
            handleWifiEnabled(false);
            } else{
                updateWifiConfig("active", false);  
            }

        } else if (!apStatus && !checked) {
            showErrorAlert('No puedes desactivar la red WiFi sin activar el modo AP');
        }else if (checked) {
            updateWifiConfig("active", true);
            
        }
    };
    const handleWifiEnabled = (enabled) => {
        switch (enabled) {
            case false:
                if (webSocket) {
                    let message = { action: 'SETWIFICONFIG', wifiActive: false };
                    webSocket.send(JSON.stringify(message));
                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            default:
            // Lógica por defecto
        }
    };

    //FORMATO DE IP, MÁSCARA DE SUBRED Y PUERTA DE ENLACE
    const format = (inputValue) => {
        let formattedValue = '';
        let blockCount = 0;
        let currentBlock = '';

        for (let i = 0; i < inputValue.length; i++) {
            const char = inputValue[i];

            if (char === '.') {
                if (blockCount < 3) {
                    formattedValue += currentBlock + '.';
                    currentBlock = '';
                    blockCount++;
                }
            } else if (char >= '0' && char <= '9') {
                if (currentBlock.length < 3) {
                    currentBlock += char;
                }
            }
        }

        formattedValue += currentBlock; // Agregar el último bloque sin punto

        return formattedValue;
    };

    const handleStaticWifiIPChange = (e) => {
        const inputValue = e.target.value.replace(/[^\d.]/g, ''); // Eliminar caracteres que no sean dígitos o puntos
        const formattedValue = format(inputValue);
        updateWifiConfig("ip", formattedValue);
    };

    const handleStaticSubnetMaskChange = (e) => {
        const inputValue = e.target.value.replace(/[^\d.]/g, ''); // Eliminar caracteres que no sean dígitos o puntos
        const formattedValue = format(inputValue);
        updateWifiConfig("subnet", formattedValue);
    }

    const handleStaticGatewayChange = (e) => {
        const inputValue = e.target.value.replace(/[^\d.]/g, ''); // Eliminar caracteres que no sean dígitos o puntos
        const formattedValue = format(inputValue);
        updateWifiConfig("gateway", formattedValue);
    }
    const calculateSignalStrength = (rssi) => {
        // El rango típico de RSSI va de -100 a 0 dBm
        // Suponiendo que -100 dBm es la señal más débil y 0 dBm es la señal más fuerte
        // Podríamos escalar el valor de RSSI a un porcentaje de 0 a 100
        // Por ejemplo, si RSSI es -100, el porcentaje sería 0, y si RSSI es 0, el porcentaje sería 100
    
        // Limitamos el rango de RSSI entre -100 y 0
        const boundedRSSI = Math.min(0, Math.max(-100, rssi));
    
        // Escalamos el valor de RSSI al rango de 0 a 100
        const percentage = 100 * ((boundedRSSI + 100) / 100);
    
        // Redondeamos el porcentaje al número entero más cercano
        return Math.round(percentage);
    };
    
    return (

        <Row>
            <Col md={12}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">Configuración WiFi</h5>
                        <Form.Switch
                            className="float-end"
                            checked={wifiConfig.active}
                            onChange={(e) => handleWifiEnabledChange(e.target.checked)}
                        />
                    </Card.Header>
                    <Card.Body hidden={!wifiConfig.active}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Red WiFi:</Form.Label>
                            <div className="d-flex flex-row align-items-center">
                                <Form.Select value={selectedNetwork} onChange={(e) => setSelectedNetwork(e.target.value)}>
                                    <option value="">{selectedNetwork !== '' ? selectedNetwork : 'Selecciona una red'}</option>
                                    {wifiNetworks.length > 0 ? (
                                        wifiNetworks.map((network, index) => (
                                            <option key={index} value={network.ssid}>
                                                {`${network.ssid} (${calculateSignalStrength(network.rssi)}%) `}
                                            </option>
                                        ))
                                    ) : (
                                        <option disabled>No se encontraron redes WiFi</option>
                                    )}
                                </Form.Select>
                                <Button onClick={handleGetNetworks} className="ms-2">
                                    {isLoadingSearchNetworks ?
                                        <Spinner animation="border" size="sm" /> :
                                        <BsSearchHeart />
                                    }
                                </Button>

                            </div>
                        </Form.Group>


                        <Form.Group className="mb-3">
                            <Form.Label >Contraseña:</Form.Label>
                            <div className="d-flex flex-row align-items-center">
                                <Form.Control type={showWifiPassword ? "text" : "password"} value={wifiConfig.password} onChange={(e) => updateWifiConfig("password", e.target.value)} />
                                <Button className="ms-2" onClick={() => setShowWifiPassword(!showWifiPassword)}>{showWifiPassword ? <BsEyeSlash /> : <BsEye />}</Button>
                            </div>
                        </Form.Group>

                    </Card.Body>
                    <div hidden={!wifiConfig.active}>
                    <Card.Header className={`d-flex flex-row justify-content-between align-items-center `} >
                        <h5 className="mb-0">IP Estática:</h5>
                    </Card.Header>

                    <Card.Body  >

                        <Form.Group className="mb-3">
                            <Form.Label >Dirección IP:</Form.Label>
                            <Form.Control
                                type="text"
                                value={wifiConfig.ip}
                                onChange={handleStaticWifiIPChange}
                                required
                            />
                        </Form.Group>
                        <Form.Group className="mb-3">
                            <Form.Label >Máscara de Subred:</Form.Label>
                            <Form.Control
                                type="text"
                                value={wifiConfig.subnet}
                                onChange={handleStaticSubnetMaskChange}
                                required
                            />
                        </Form.Group>
                        <Form.Group className="mb-3">
                            <Form.Label >Puerta de Enlace:</Form.Label>
                            <Form.Control
                                type="text"
                                value={wifiConfig.gateway}
                                onChange={handleStaticGatewayChange}
                                required
                            />
                        </Form.Group>

                        <Button variant="primary" onClick={() => setConfigWifi()} className="d-flex align-items-center"> {isLoadingSetWifiConfig ?
                            <Spinner animation="border" size="sm" /> :
                            <>
                                <FaUpload className="me-1" />
                                Cargar
                            </>
                        } </Button>
                    </Card.Body>
                    </div>
                </Card>
            </Col>
        </Row>

    );
}

export default WifiConfigComponent;
