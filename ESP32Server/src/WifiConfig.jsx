import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const WifiConfigComponent = ({ wifiConfig, apStatus, setWifiStatusConfig, webSocket }) => {

    // ESTADOS PARA LA CONEXIÓN DE RED
    const [isLoadingSearchNetworks, setIsLoadingSearchNetworks] = useState(false); // Estado para mostrar el spinner de búsqueda de redes
    const [isLoadingSetWifiConfig, setIsLoadingSetWifiConfig] = useState(false);   // Estado para mostrar el spinner de configuración de WiFi
    // ESTADOS PARA LA CONFIGURACIÓN DE LA RED WIFI
    const [wifiEnabled, setWifiEnabled] = useState(wifiConfig.status);             // Estado para el estado de la conexión WiFi
    const [wifiStatus, setWifiStatus] = useState(wifiConfig.status);               // Estado para el estado de la conexión WiFi
    const [wifiSSID, setWifiSSID] = useState(wifiConfig.ssid);                     // Estado para el SSID de la red WiFi
    const [wifiPassword, setWifiPassword] = useState(wifiConfig.password);         // Estado para la contraseña de la red WiFi
    const [showWifiPassword, setShowWifiPassword] = useState(false);               // Estado para mostrar u ocultar la contraseña del WiFi
    // ESTADOS PARA LA BÚSQUEDA DE REDES
    const [networks, setNetworks] = useState([]);                                  // Estado para almacenar las redes disponibles
    const [selectedNetwork, setSelectedNetwork] = useState('');                    // Estado para almacenar la red seleccionada
    //ESTADOS PARA LA CONFIGURACIÓN DE IP ESTÁTICA
    const [staticIPEnabled, setStaticIPEnabled] = useState(wifiConfig.staticIp);   // Estado para habilitar la configuración de IP estática
    const [staticWifiIP, setStaticWifiIP] = useState(wifiConfig.ip);               // Estado para la dirección IP estática
    const [staticSubnetMask, setStaticSubnetMask] = useState(wifiConfig.subnet);   // Estado para la máscara de subred estática
    const [staticGateway, setStaticGateway] = useState(wifiConfig.gateway);        // Estado para la puerta de enlace estática


    // OBTENER LAS REDES DISPONIBLES
    const handleGetNetworks = () => {
        if (webSocket) {
            setIsLoadingSearchNetworks(true);
            let message = { action: 'getNetworks' };
            webSocket.send(JSON.stringify(message));
            webSocket.onmessage = (event) => {
                const data = JSON.parse(event.data);
                setNetworks(data.networks);
                setIsLoadingSearchNetworks(false);
            };
        } else {
            console.log('WebSocket no está inicializado');
        }
    };
    //CONFIGURAR WIFI
    const setConfigWifi = () => {
        setIsLoadingSetWifiConfig(true);
        if (selectedNetwork === '' || wifiPassword === '' || staticWifiIP === '' || staticSubnetMask === '' || staticGateway === '') {
            showErrorAlert('Debes llenar todos los campos');
            setIsLoadingSetWifiConfig(false);
            return;
        }
        if (webSocket) {
            let message = { action: 'setWifiConfig', wifiActive: true, ssid: selectedNetwork, password: wifiPassword, ip: staticWifiIP, subnet: staticSubnetMask, gateway: staticGateway };
            webSocket.send(JSON.stringify(message));
            webSocket.onmessage = (event) => {
                const data = event.data;
                const dataJson = JSON.parse(data);
                const status = dataJson.status;
                if (status === 'true') {
                    const IP = dataJson.ip;
                    const Subnet = dataJson.subnet;
                    const Gateway = dataJson.gateway;
                    setWifiSSID(selectedNetwork);
                    setStaticWifiIP(IP);
                    setStaticSubnetMask(Subnet);
                    setStaticGateway(Gateway);
                    showSuccessAlert('Conectado correctamente');
                    setWifiStatus(true);
                    setWifiStatusConfig(true);
                    setIsLoadingSetWifiConfig(false);
                } else if (status === 'false') {
                    showErrorAlert('Error al conectar');
                    setWifiStatus(false);
                    setWifiSSID('');
                    setWifiPassword('');
                    setIsLoadingSetWifiConfig(false);

                }
            };


        } else {
            console.log('WebSocket no está inicializado');
        }
    };
    //ACTIVAR O DESACTIVAR LA RED WIFI
    const handleWifiEnabledChange = (checked) => {
        setWifiEnabled(checked);

        if (apStatus && !checked) {
            handleWifiEnabled(checked);
        } else if (!apStatus && !checked) {
            showErrorAlert('No puedes desactivar la red WiFi sin activar el modo AP');
            setWifiEnabled(true);
        }
    };

    const handleWifiEnabled = (enabled) => {
        switch (enabled) {
            case false:
                if (webSocket) {
                    let message = { action: 'setWifiConfig', wifiActive: false };
                    setWifiSSID('');
                    setWifiPassword('');
                    setStaticWifiIP('');
                    setStaticSubnetMask('');
                    setStaticGateway('');
                    webSocket.send(JSON.stringify(message));
                    setWifiStatus(false);
                    setWifiEnabled(false);
                    setWifiStatusConfig(false);
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
        setStaticWifiIP(formattedValue);
    };

    const handleStaticSubnetMaskChange = (e) => {
        const inputValue = e.target.value.replace(/[^\d.]/g, ''); // Eliminar caracteres que no sean dígitos o puntos
        const formattedValue = format(inputValue);
        setStaticSubnetMask(formattedValue);
    }

    const handleStaticGatewayChange = (e) => {
        const inputValue = e.target.value.replace(/[^\d.]/g, ''); // Eliminar caracteres que no sean dígitos o puntos
        const formattedValue = format(inputValue);
        setStaticGateway(formattedValue);
    }
    return (

        <Row>
            <Col md={12}>
                <Card className="mb-3">
                    <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                        <h5 className="mb-0">Configuración WiFi</h5>
                        <Form.Switch
                            className="float-end"
                            checked={wifiEnabled}
                            onChange={(e) => handleWifiEnabledChange(e.target.checked)}
                        />
                    </Card.Header>
                    <Card.Body hidden={!wifiEnabled}>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Red WiFi:</Form.Label>
                            <div className="d-flex flex-row align-items-center">
                                <Form.Select value={selectedNetwork} onChange={(e) => setSelectedNetwork(e.target.value)} >
                                    <option value={wifiSSID}>{wifiSSID !== '' ? wifiSSID : 'Selecciona una red'}</option>

                                    {networks.map((network, index) => (
                                        <option key={index} value={network}>{network}</option>
                                    ))}
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
                                <Form.Control type={showWifiPassword ? "text" : "password"} value={wifiPassword} onChange={(e) => setWifiPassword(e.target.value)} />
                                <Button className="ms-2" onClick={() => setShowWifiPassword(!showWifiPassword)}>{showWifiPassword ? <BsEyeSlash /> : <BsEye />}</Button>
                            </div>
                        </Form.Group>

                    </Card.Body>
                    <Card.Header className={`d-flex flex-row justify-content-between align-items-center ${!wifiEnabled && 'd-none'}`}>
                        <h5 className="mb-0">IP Estática:</h5>
                    </Card.Header>

                    <Card.Body hidden={!wifiEnabled} >

                        <Form.Group className="mb-3">
                            <Form.Label >Dirección IP:</Form.Label>
                            <Form.Control
                                type="text"
                                value={staticWifiIP}
                                onChange={handleStaticWifiIPChange}
                                required
                            />
                        </Form.Group>
                        <Form.Group className="mb-3">
                            <Form.Label >Máscara de Subred:</Form.Label>
                            <Form.Control
                                type="text"
                                value={staticSubnetMask}
                                onChange={handleStaticSubnetMaskChange}
                                required
                            />
                        </Form.Group>
                        <Form.Group className="mb-3">
                            <Form.Label >Puerta de Enlace:</Form.Label>
                            <Form.Control
                                type="text"
                                value={staticGateway}
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
                </Card>
            </Col>
        </Row>

    );
}

export default WifiConfigComponent;
