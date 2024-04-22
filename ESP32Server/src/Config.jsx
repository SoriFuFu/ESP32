import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash } from "react-icons/bs";
import { FaUpload } from "react-icons/fa";
import { showErrorAlert, showSuccessAlert } from './alerts';

const Config = () => {
    const [isLoading, setIsLoading] = useState(false); // Estado para el loading de la página
    const [isLoadingSearchNetworks, setIsLoadingSearchNetworks] = useState(false);
    const [isLoadingSetWifiConfig, setIsLoadingSetWifiConfig] = useState(false);
    const [showWifiPassword, setShowWifiPassword] = useState(false); // Estado para mostrar u ocultar la contraseña del WiFi
    const [showApPassword, setShowApPassword] = useState(false); // Estado para mostrar u ocultar la contraseña del AP
    const [wifiEnabled, setWifiEnabled] = useState(false);
    const [wifiStatus, setWifiStatus] = useState(''); // Estado para el estado de la conexión WiFi
    const [wifiSSID, setWifiSSID] = useState('');
    const [wifiPassword, setWifiPassword] = useState('');
    const [networks, setNetworks] = useState([]);
    const [selectedNetwork, setSelectedNetwork] = useState('');

    const [staticIPEnabled, setStaticIPEnabled] = useState(false);
    const [staticWifiIP, setStaticWifiIP] = useState('');
    const [prevStaticWifiIP, setPrevStaticWifiIP] = useState('');
    const [staticSubnetMask, setStaticSubnetMask] = useState('');
    const [prevStaticSubnetMask, setPrevStaticSubnetMask] = useState('');
    const [staticGateway, setStaticGateway] = useState('');
    const [prevStaticGateway, setPrevStaticGateway] = useState('');

    const [apEnabled, setApEnabled] = useState(false);
    const [apStatus, setApStatus] = useState(''); // Estado para el estado de la conexión AP
    const [apName, setApName] = useState('');
    const [apPassword, setApPassword] = useState('');


    const [K1Enabled, setK1Enabled] = useState(false);
    const [K1Name, setK1Name] = useState('');
    const [K1Mode, setK1Mode] = useState('');
    const [K2Enabled, setK2Enabled] = useState(false);
    const [K2Name, setK2Name] = useState('');
    const [K2Mode, setK2Mode] = useState('');
    const [K3Enabled, setK3Enabled] = useState(false);
    const [K3Name, setK3Name] = useState('');
    const [K3Mode, setK3Mode] = useState('');
    const [K4Enabled, setK4Enabled] = useState(false);
    const [K4Name, setK4Name] = useState('');
    const [K4Mode, setK4Mode] = useState('');
    const [webSocket, setWebSocket] = useState(null);
    const [clientStart, setClientStart] = useState(false);

    useEffect(() => {
        const ws = new WebSocket('ws://192.168.0.101:81');
        // const ws = new WebSocket('ws://192.168.4.1:81');
        // const ws = new WebSocket('ws://' + window.location.hostname + ':81');
        setWebSocket(ws);

        ws.onopen = () => {
            console.log('Conexión WebSocket establecida');
            let message = { action: 'getConfig' };
            ws.send(JSON.stringify(message));
        };

        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            setWifiEnabled(data.Wifi.active);
            setStaticIPEnabled(data.Wifi.staticIp);
            setWifiSSID(data.Wifi.ssid);
            setWifiPassword(data.Wifi.password);
            setWifiStatus(data.Wifi.status);

            setStaticWifiIP(data.Wifi.ip);
            setPrevStaticWifiIP(data.Wifi.ip);
            setStaticSubnetMask(data.Wifi.subnet);
            setPrevStaticSubnetMask(data.Wifi.subnet);
            setStaticGateway(data.Wifi.gateway);
            setPrevStaticGateway(data.Wifi.gateway);

            setApEnabled(data.AP.active);
            setApStatus(data.AP.status);
            setApName(data.AP.ssid);
            setApPassword(data.AP.password);

            setK1Enabled(data.K1.active);
            setK1Name(data.K1.name);
            setK1Mode(data.K1.mode);
            setK2Enabled(data.K2.active);
            setK2Name(data.K2.name);
            setK2Mode(data.K2.mode);
            setK3Enabled(data.K3.active);
            setK3Name(data.K3.name);
            setK3Mode(data.K3.mode);
            setK4Enabled(data.K4.active);
            setK4Name(data.K4.name);
            setK4Mode(data.K4.mode);

            setIsLoading(false);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket cerrada');
        };

        return () => {
            if (ws) {
                ws.close();
            }
        };
    }, []);

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
    //CONECTAR A LA RED WIFI
    const setConfigWifi = () => {
        setIsLoadingSetWifiConfig(true);
        if (webSocket) {
            let message = { action: 'setWifiConfig', wifiActive: true, ssid: selectedNetwork, password: wifiPassword };
            webSocket.send(JSON.stringify(message));
            webSocket.onmessage = (event) => {
                const data = event.data;
                const dataJson = JSON.parse(data);
                const status = dataJson.status;
                if (status === 'true') {
                const IP = dataJson.ip;
                const Subnet = dataJson.subnet;
                const Gateway = dataJson.gateway;
                    setStaticWifiIP(IP);
                    setStaticSubnetMask(Subnet);
                    setStaticGateway(Gateway);
                    showSuccessAlert('Conectado correctamente');
                    setWifiStatus(true);
                    setIsLoadingSetWifiConfig(false);
                } else if (status === 'false'){
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
    }
    // DESACTIVAR LA IP ESTÁTICA
    
    useEffect(() => {
        if (staticWifiIP !== prevStaticWifiIP && staticSubnetMask !== prevStaticSubnetMask && staticGateway !== prevStaticGateway) {
            if (!staticIPEnabled) {
                setStaticWifiIP('');
                setStaticSubnetMask('');
                setStaticGateway('');
                if (webSocket) {
                    let message = { action: 'setStaticIp', staticIp: false };
                    webSocket.send(JSON.stringify(message)); // Envía true si staticIPEnabled es false
                };
            } else {
                console.log('WebSocket no está inicializado');
            }
        }
    }, [staticIPEnabled]);
    //CONFIGURAR IP ESTÁTICA
        const handleIpStaticChange = (checked) => {
        if (wifiStatus) {
            handleIpStaticDisabled(checked);
        } 
    };
    const setConfigStaticIP = () => {

        if (webSocket) {
            let message = { action: 'setStaticIp', staticIp: true, ip: staticWifiIP, subnet: staticSubnetMask, gateway: staticGateway };
            webSocket.send(JSON.stringify(message)); // Envía true si staticIPEnabled es true
            webSocket.onmessage = (event) => {
                if (event.data === 'true') {
                    showSuccessAlert('IP Estática configurada correctamente');
                } else {
                    showErrorAlert('Error al configurar IP Estática');
                }
            };
        } else {
            console.log('WebSocket no está inicializado');
        }
    }
    //ACTIVAR O DESACTIVAR EL MODO AP
  
    const handleApEnabledChange = (checked) => {
        if (wifiStatus) {
            handleApEnabled(checked);
        } else if (!wifiStatus) {
            showErrorAlert('No puedes desactivar la red AP sin conectar a una red WiFi');
            setApEnabled(true);
            
        }
    };

    const handleApEnabled = (enabled) => {
        switch (enabled) {
            case true:
                if (webSocket) {
                    let message = { action: 'setApConfig', active: true };
                    webSocket.send(JSON.stringify(message));
                    setApStatus(true);
                    setApEnabled(true);

                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            case false:
                if (webSocket) {
                    let message = { action: 'setApConfig', active: false };
                    webSocket.send(JSON.stringify(message));
                    setApStatus(false);
                    setApEnabled(false);
                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            default:
            
        }
    };
    //DESACTIVAR WIFI
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
                    webSocket.send(JSON.stringify(message));
                    setWifiStatus(false); 
                    setWifiEnabled(false);  
                } else {
                    console.log('WebSocket no está inicializado');
                }
                break;
            default:
            // Lógica por defecto
        }
    };

    const setRelayConfig = (relay) => {
        if (webSocket) {
            if (relay === 1) {
                console.log('K1');
                let message = { action: 'setRelayConfig', relay: relay, K1Active: true, relayName: K1Name, relayMode: K1Mode };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 2) {
                let message = { action: 'setRelayConfig', relay: relay, K2Active: true, relayName: K2Name, relayMode: K2Mode };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 3) {
                let message = { action: 'setRelayConfig', relay: relay, K3Active: true, relayName: K3Name, relayMode: K3Mode };
                webSocket.send(JSON.stringify(message));
            } else if (relay === 4) {
                let message = { action: 'setRelayConfig', relay: relay, K4Active: true, relayName: K4Name, relayMode: K4Mode };
                webSocket.send(JSON.stringify(message));
            }
            
        } else {
            console.log('WebSocket no está inicializado');
        }
    }
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
        <>
            {/* Elemento de carga condicional */}
            {isLoading ? (
                <div className="d-flex justify-content-center align-items-center" style={{ minHeight: '100vh' }}>
                    <Spinner animation="border" role="status">
                        <span className="visually-hidden">Cargando...</span>
                    </Spinner>
                </div>
            ) : (
                // Contenido del componente
                <>

                    <Row className="mb-3">
                        <Col>
                            <h2>Configuración</h2>
                        </Col>

                    </Row>

                    {/* Sección de Configuración WiFi */}
                    <Row>
                        <Col md={6}>
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
                                                <option value="">{wifiSSID !== '' ? wifiSSID : 'Selecciona una red'}</option>

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
                                    <Button variant="primary" onClick={() => setConfigWifi()} className="d-flex align-items-center"> {isLoadingSetWifiConfig ?
                                                    <Spinner animation="border" size="sm" /> :
                                                    <>
                                                    <FaUpload className="me-1" />
                                                    Cargar
                                                </>
                                                } </Button>
                                </Card.Body>
                                <Card.Header className={`d-flex flex-row justify-content-between align-items-center ${!wifiEnabled && 'd-none'}`}>
                                    <h5 className="mb-0">IP Estática:</h5>
                                    <Form.Switch checked={staticIPEnabled} onChange={(e) => setStaticIPEnabled(e.target.checked)} required />
                                </Card.Header>

                                <Card.Body hidden={!staticIPEnabled}>

                                    <Form.Group className="mb-3">
                                        <Form.Label >Dirección IP:</Form.Label>
                                        <Form.Control
                                            type="text"
                                            value={staticWifiIP}
                                            onChange={handleStaticWifiIPChange}
                                            required={staticIPEnabled}
                                        />
                                    </Form.Group>
                                    <Form.Group className="mb-3">
                                        <Form.Label >Máscara de Subred:</Form.Label>
                                        <Form.Control
                                            type="text"
                                            value={staticSubnetMask}
                                            onChange={handleStaticSubnetMaskChange}
                                            required={staticIPEnabled}
                                        />
                                    </Form.Group>
                                    <Form.Group className="mb-3">
                                        <Form.Label >Puerta de Enlace:</Form.Label>
                                        <Form.Control
                                            type="text"
                                            value={staticGateway}
                                            onChange={handleStaticGatewayChange}
                                            required={staticIPEnabled}
                                        />
                                    </Form.Group>

                                    <Button variant="primary" onClick={() => setConfigStaticIP()} disabled={!wifiEnabled} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                                </Card.Body>
                            </Card>
                        </Col>
                        <Col md={6}>
                            {/* Sección de Configuración AP */}
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">Modo AP</h5>
                                    <Form.Switch className="float-end" checked={apEnabled} onChange={(e) => handleApEnabledChange(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!apEnabled}>
                                    <Form.Group className="mb-3">
                                        <Form.Label >SSID:</Form.Label>

                                        <Form.Control type="text" value={apName} onChange={(e) => setApName(e.target.value)} disabled />

                                    </Form.Group>
                                    <Form.Group className="mb-3" >
                                        <Form.Label>Contraseña:</Form.Label>
                                        <div className="d-flex flex-row align-items-center">
                                            <Form.Control type={showApPassword ? "text" : "password"} value={apPassword} onChange={(e) => setApPassword(e.target.value)} disabled />

                                            <Button className="ms-2" onClick={() => setShowApPassword(!showApPassword)}>{showApPassword ? <BsEyeSlash /> : <BsEye />}</Button>
                                        </div>
                                    </Form.Group>
                                    {/* <Button variant="primary" onClick={() => setAPConfig()} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button> */}
                                </Card.Body>
                            </Card>
                        </Col>

                        {/* Sección de Configuración de Relés */}
                        {/* Agrega aquí la sección de configuración de los relés */}
                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">K1</h5>
                                    <Form.Switch className="float-end" checked={K1Enabled} onChange={(e) => setK1Enabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!K1Enabled}>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Nombre</Form.Label>
                                        <Col sm={9}>
                                        <Form.Control type="text" value={K1Name} onChange={(e) => setK1Name(e.target.value)} />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                        <Col sm={9}>
                                            <Form.Select >
                                                <option value="1">Temporizador regresivo</option>
                                                <option value="2">Programación diaria</option>
                                                <option value="3">Enclavamiento</option>
                                                <option value="4">Activación por Eventos Externos</option>
                                            </Form.Select>
                                        </Col>
                                    </Form.Group>
                                    <Button variant="primary" onClick={() => setRelayConfig(1)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                                </Card.Body>
                            </Card>
                        </Col>

                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">K2</h5>
                                    <Form.Switch className="float-end" checked={K2Enabled} onChange={(e) => setK2Enabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!K2Enabled}>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Nombre</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={K2Name} disabled={!K2Enabled} />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                        <Col sm={9}>
                                            <Form.Select disabled={!K2Enabled}>
                                                <option value="1">Temporizador regresivo</option>
                                                <option value="2">Programación diaria</option>
                                                <option value="3">Enclavamiento</option>
                                                <option value="4">Activación por Eventos Externos</option>
                                            </Form.Select>
                                        </Col>
                                    </Form.Group>
                                    <Button variant="primary" onClick={() => setRelayConfig(2)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                                </Card.Body>
                            </Card>
                        </Col>

                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">K3</h5>
                                    <Form.Switch className="float-end" checked={K3Enabled} onChange={(e) => setK3Enabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!K3Enabled}>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Nombre</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={K3Name} disabled={!K3Enabled} />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                        <Col sm={9}>
                                            <Form.Select disabled={!K3Enabled}>
                                                <option value="1">Temporizador regresivo</option>
                                                <option value="2">Programación diaria</option>
                                                <option value="3">Enclavamiento</option>
                                                <option value="4">Activación por Eventos Externos</option>
                                            </Form.Select>
                                        </Col>
                                    </Form.Group>
                                    <Button variant="primary" onClick={() => setRelayConfig(3)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                                </Card.Body>
                            </Card>
                        </Col>

                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header className="d-flex flex-row justify-content-between align-items-center">
                                    <h5 className="mb-0">K4</h5>
                                    <Form.Switch className="float-end" checked={K4Enabled} onChange={(e) => setK4Enabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!K4Enabled}>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Nombre</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={K4Name} disabled={!K4Enabled} />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                        <Col sm={9}>
                                            <Form.Select disabled={!K4Enabled}>
                                                <option value="1">Temporizador regresivo</option>
                                                <option value="2">Programación diaria</option>
                                                <option value="3">Enclavamiento</option>
                                                <option value="4">Activación por Eventos Externos</option>
                                            </Form.Select>
                                        </Col>
                                    </Form.Group>
                                    <Button variant="primary" onClick={() => setRelayConfig(4)} className="d-flex align-items-center"> <FaUpload className="me-1" /> Cargar</Button>
                                </Card.Body>
                            </Card>
                        </Col>

                    </Row>
                </>
            )
            }
        </>
    );
}

export default Config;
