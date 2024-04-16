import React from 'react';
import { useState } from 'react';
import { Card, Form, Button, Container, Row, Col, ToggleButton, ToggleButtonGroup } from 'react-bootstrap';

const Config = () => {
    const [wifiEnabled, setWifiEnabled] = useState(false);
    const [apEnabled, setApEnabled] = useState(true);
    const [staticIPEnabled, setStaticIPEnabled] = useState(false);
    const [networks, setNetworks] = useState([]);
    const [selectedNetwork, setSelectedNetwork] = useState('');
    const [password, setPassword] = useState('');
    const [apName, setApName] = useState('');
    const [apPassword, setApPassword] = useState('');
    const [relay1Enabled, setRelay1Enabled] = useState(false);
    const [relay2Enabled, setRelay2Enabled] = useState(false);
    const [relay3Enabled, setRelay3Enabled] = useState(false);
    const [relay4Enabled, setRelay4Enabled] = useState(false);
    const [staticIP, setStaticIP] = useState('');
    const [subnetMask, setSubnetMask] = useState('');
    const [gatewayIP, setGatewayIP] = useState('');

    const handleSearchNetworks = () => {
        // Lógica para buscar redes y actualizar el estado "networks"
    };

    return (
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
                                <Col sm={9}>
                                    <Form.Select value={selectedNetwork} onChange={(e) => setSelectedNetwork(e.target.value)} disabled={!wifiEnabled}>
                                        <option value="">Selecciona una red</option>
                                        {networks.map((network, index) => (
                                            <option key={index} value={network}>{network}</option>
                                        ))}
                                    </Form.Select>
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Contraseña:</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="password" value={password} onChange={(e) => setPassword(e.target.value)} disabled={!wifiEnabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>IP Estática:</Form.Label>
                                <Col sm={9}>
                                    <Form.Switch checked={staticIPEnabled} onChange={(e) => setStaticIPEnabled(e.target.checked)} disabled={!wifiEnabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Dirección IP:</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" value={staticIP} onChange={(e) => setStaticIP(e.target.value)} disabled={!wifiEnabled || !staticIPEnabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Máscara de Subred:</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" value={subnetMask} onChange={(e) => setSubnetMask(e.target.value)} disabled={!wifiEnabled || !staticIPEnabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Puerta de Enlace:</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" value={gatewayIP} onChange={(e) => setGatewayIP(e.target.value)} disabled={!wifiEnabled || !staticIPEnabled} />
                                </Col>
                            </Form.Group>
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
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Contraseña del AP:</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="password" value={apPassword} onChange={(e) => setApPassword(e.target.value)} disabled={!apEnabled} />
                                </Col>
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
                            <Form.Switch className="float-end" checked={relay1Enabled} onChange={(e) => setRelay1Enabled(e.target.checked)} />
                        </Card.Header>
                        <Card.Body>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Nombre</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" disabled={!relay1Enabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                <Col sm={9}>
                                    <Form.Select disabled={!relay1Enabled}>
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
                            <Form.Switch className="float-end" checked={relay2Enabled} onChange={(e) => setRelay2Enabled(e.target.checked)} />
                        </Card.Header>
                        <Card.Body>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Nombre</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" disabled={!relay2Enabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                <Col sm={9}>
                                    <Form.Select disabled={!relay2Enabled}>
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
                            <Form.Switch className="float-end" checked={relay3Enabled} onChange={(e) => setRelay3Enabled(e.target.checked)} />
                        </Card.Header>
                        <Card.Body>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Nombre</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" disabled={!relay3Enabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                <Col sm={9}>
                                    <Form.Select disabled={!relay3Enabled}>
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
                            <Form.Switch className="float-end" checked={relay4Enabled} onChange={(e) => setRelay4Enabled(e.target.checked)} />
                        </Card.Header>
                        <Card.Body>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Nombre</Form.Label>
                                <Col sm={9}>
                                    <Form.Control type="text" disabled={!relay4Enabled} />
                                </Col>
                            </Form.Group>
                            <Form.Group as={Row} className="mb-3">
                                <Form.Label column sm={3}>Modo de funcionamieno</Form.Label>
                                <Col sm={9}>
                                    <Form.Select disabled={!relay4Enabled}>
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

    );
}

export default Config;
