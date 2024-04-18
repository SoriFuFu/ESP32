import React, { useState, useEffect } from 'react';
import { Card, Form, Button, Row, Col, Spinner } from 'react-bootstrap';
import { BsSearchHeart, BsEye, BsEyeSlash    } from "react-icons/bs";

const Config = () => {
    const [isLoading, setIsLoading] = useState(false); // Estado para el loading
    const [showWifiPassword, setShowWifiPassword] = useState(false); // Estado para mostrar u ocultar la contraseña del WiFi
    const [showApPassword, setShowApPassword] = useState(false); // Estado para mostrar u ocultar la contraseña del AP
    const [wifiEnabled, setWifiEnabled] = useState(false);
    const [wifiSSID, setWifiSSID] = useState('');
    const [wifiPassword, setWifiPassword] = useState('');
    const [staticIPEnabled, setStaticIPEnabled] = useState(false);
    const [staticWifiIP, setStaticWifiIP] = useState('');
    const [staticSubnetMask, setStaticSubnetMask] = useState('');
    const [staticGateway, setStaticGateway] = useState('');
    const [apEnabled, setApEnabled] = useState(true);
    const [apName, setApName] = useState('');
    const [apPassword, setApPassword] = useState('');
    const [networks, setNetworks] = useState([]);
    const [selectedNetwork, setSelectedNetwork] = useState('');

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

    useEffect(() => {
        const ws = new WebSocket('ws://192.168.1.38:81');
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
            setStaticWifiIP(data.Wifi.ip);
            setStaticSubnetMask(data.Wifi.subnet);
            setStaticGateway(data.Wifi.gateway);

            setApEnabled(data.AP.active);
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

    const handleGetNetworks = () => {
        if (webSocket) {
            console.log('Enviando mensaje getNetworks');
            let message = { action: 'getNetworks' };
            webSocket.send(JSON.stringify(message));
            webSocket.onmessage = (event) => {
                const data = JSON.parse(event.data);
                setNetworks(data.networks);
            };
        } else {
            console.log('WebSocket no está inicializado');
        }
    };

   
    useEffect(() => {
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
    }, [staticIPEnabled]); // Asegúrate de incluir staticIPEnabled en la lista de dependencias
    
    const setConfigStaticIP = () => {
        if (webSocket) {
            let message = { action: 'setStaticIp', staticIp: true, ip: staticWifiIP, subnet: staticSubnetMask, gateway: staticGateway };
            webSocket.send(JSON.stringify(message)); // Envía true si staticIPEnabled es true
        } else {
            console.log('WebSocket no está inicializado');
        }
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
                        <Col className="d-flex justify-content-end align-items-center">
                            <Button variant="primary">Guardar</Button>
                        </Col>
                    </Row>

                    {/* Sección de Configuración WiFi */}
                    <Row>
                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header>
                                    <h5 className="mb-0">Configuración WiFi</h5>
                                    <Form.Switch className="float-end" checked={wifiEnabled} onChange={(e) => setWifiEnabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Red WiFi:</Form.Label>
                                        <div className="d-flex flex-row align-items-center">
                                            <Form.Select value={selectedNetwork} onChange={(e) => setSelectedNetwork(e.target.value)} disabled={!wifiEnabled}>
                                                <option value="">Selecciona una red</option>
                                                {networks.map((network, index) => (
                                                    <option key={index} value={network}>{network}</option>
                                                ))}
                                            </Form.Select>
                                            <Button onClick={handleGetNetworks} disabled={!wifiEnabled} className="ms-2"><BsSearchHeart /></Button>
                                        </div>
                                    </Form.Group>



                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Contraseña:</Form.Label>
                                        <div className="d-flex flex-row align-items-center">
                                            <Form.Control type={showWifiPassword ? "text" : "password"} value={wifiPassword} onChange={(e) => setWifiPassword(e.target.value)} disabled={!wifiEnabled} />
                                            <Button className="ms-2" onClick={() => setShowWifiPassword(!showWifiPassword)}>{showWifiPassword ? <BsEyeSlash /> : <BsEye/>}</Button>
                                        </div>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>IP Estática:</Form.Label>
                                        <Col sm={9}>
                                            <Form.Switch checked={staticIPEnabled} onChange={(e) => setStaticIPEnabled(e.target.checked)} disabled={!wifiEnabled} required />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Dirección IP:</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={staticWifiIP} onChange={(e) => setStaticWifiIP(e.target.value)} disabled={!wifiEnabled || !staticIPEnabled} required/>
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Máscara de Subred:</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={staticSubnetMask} onChange={(e) => setStaticSubnetMask(e.target.value)} disabled={!wifiEnabled || !staticIPEnabled} required/>
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Puerta de Enlace:</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={staticGateway} onChange={(e) => setStaticGateway(e.target.value)} disabled={!wifiEnabled || !staticIPEnabled} required/>
                                        </Col>
                                    </Form.Group>

                                    <Button variant="primary" onClick={() => setConfigStaticIP()} disabled={!wifiEnabled}>Configurar</Button>
                                </Card.Body>
                            </Card>
                        </Col>
                        <Col md={6}>
                            {/* Sección de Configuración AP */}
                            <Card className="mb-3">
                                <Card.Header>
                                    <h5 className="mb-0">Modo AP</h5>
                                    <Form.Switch className="float-end" checked={apEnabled} onChange={(e) => setApEnabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Nombre del AP:</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={apName} onChange={(e) => setApName(e.target.value)} disabled={!apEnabled} />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} >
                                        <Form.Label column sm={3}>Contraseña del AP:</Form.Label>
                                        <div className="d-flex flex-row align-items-center">
                                            <Form.Control type={showApPassword ? "text" : "password"} value={apPassword} onChange={(e) => setApPassword(e.target.value)} disabled={!apEnabled} />
                                    
                                            <Button className="ms-2" onClick={() => setShowApPassword(!showApPassword)}>{showApPassword ? <BsEyeSlash /> : <BsEye/>}</Button>
                                        </div>
                                    </Form.Group>
                                </Card.Body>
                            </Card>
                        </Col>

                        {/* Sección de Configuración de Relés */}
                        {/* Agrega aquí la sección de configuración de los relés */}
                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header>
                                    <h5 className="mb-0">K1</h5>
                                    <Form.Switch className="float-end" checked={K1Enabled} onChange={(e) => setK1Enabled(e.target.checked)} />
                                </Card.Header>
                                <Card.Body hidden={!K1Enabled}>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Nombre</Form.Label>
                                        <Col sm={9}>
                                            <Form.Control type="text" value={K1Name} disabled={!K1Enabled} />
                                        </Col>
                                    </Form.Group>
                                    <Form.Group as={Row} className="mb-3">
                                        <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                        <Col sm={9}>
                                            <Form.Select disabled={!K1Enabled}>
                                                <option value="1">Temporizador regresivo</option>
                                                <option value="2">Programación diaria</option>
                                                <option value="3">Enclavamiento</option>
                                                <option value="4">Activación por Eventos Externos</option>
                                            </Form.Select>
                                        </Col>
                                    </Form.Group>
                                    {/* Agrega aquí los otros campos de configuración específicos del relé 1 */}
                                </Card.Body>
                            </Card>
                        </Col>

                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header>
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
                                    {/* Agrega aquí los otros campos de configuración específicos del relé 2 */}
                                </Card.Body>
                            </Card>
                        </Col>

                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header>
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
                                    {/* Agrega aquí los otros campos de configuración específicos del relé 3 */}
                                </Card.Body>
                            </Card>
                        </Col>

                        <Col md={6}>
                            <Card className="mb-3">
                                <Card.Header>
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

                                </Card.Body>
                            </Card>
                        </Col>

                    </Row>
                </>
            )}
        </>
    );
}

export default Config;
