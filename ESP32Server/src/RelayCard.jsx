import React, { useState } from 'react';
import { Card, Form, Button, Row, Col } from 'react-bootstrap';
import { FaPowerOff } from 'react-icons/fa';

const RelayCard = ({ relay, relayKey }) => {
    const [isOn, setIsOn] = useState(false);
    const [relayData, setRelayData] = useState(relay.mode);
    const [hours, setHours] = useState(0);
    const [minutes, setMinutes] = useState(0);

    const handleToggle = () => {
        setIsOn(!isOn);
    };

    const handleIncrementMinutes = (increment) => {
        setMinutes((prevMinutes) => {
            let newMinutes = prevMinutes + increment;
            if (newMinutes < 0) {
                newMinutes = 59;
            } else if (newMinutes > 59) {
                newMinutes = 0;
            }
            return newMinutes;
        });
    };

    const handleIncrementHours = (increment) => {
        setHours((prevHours) => {
            let newHours = prevHours + increment;
            if (newHours < 0) {
                newHours = 23;
            } else if (newHours > 23) {
                newHours = 0;
            }
            return newHours;
        });
    };

    const handleHoursChange = (event) => {
        setHours(parseInt(event.target.value));
    };

    const handleMinutesChange = (event) => {
        setMinutes(parseInt(event.target.value));
    };

    const renderModeContent = () => {
        switch (relayData) {
            case "1": // Temporizador regresivo
                return (
                    <>

                        {/* Contador interactivo para modificar el tiempo regresivo */}
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Tiempo Regresivo</Form.Label>
                            <Col sm={9}>
                                <div className="d-flex align-items-center">
                                    <Button variant="outline-secondary" onClick={() => handleIncrementHours(-1)}> - </Button>
                                    <Form.Control type="number" value={hours} onChange={handleHoursChange} className="mx-2" />
                                    <Button variant="outline-secondary" onClick={() => handleIncrementHours(1)}> + </Button>
                                    <span className="mx-2">:</span>
                                    <Button variant="outline-secondary" onClick={() => handleIncrementMinutes(-1)}> - </Button>
                                    <Form.Control type="number" value={minutes} onChange={handleMinutesChange} className="mx-2" />
                                    <Button variant="outline-secondary" onClick={() => handleIncrementMinutes(1)}> + </Button>
                                </div>
                            </Col>
                        </Form.Group>
                    </>
                );

            case "2": // Programación diaria
                return (
                    <>

                        {/* Checkboxes para los días de la semana */}
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Días</Form.Label>
                            <Col sm={9}>
                                <Form.Check inline label="Lunes" type="checkbox" id="inline-checkbox-1" />
                                <Form.Check inline label="Martes" type="checkbox" id="inline-checkbox-2" />
                                <Form.Check inline label="Miércoles" type="checkbox" id="inline-checkbox-3" />
                                <Form.Check inline label="Jueves" type="checkbox" id="inline-checkbox-4" />
                                <Form.Check inline label="Viernes" type="checkbox" id="inline-checkbox-5" />
                                <Form.Check inline label="Sábado" type="checkbox" id="inline-checkbox-6" />
                                <Form.Check inline label="Domingo" type="checkbox" id="inline-checkbox-7" />
                            </Col>
                        </Form.Group>
                        {/* Campos para las horas de inicio y fin */}
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Hora de inicio</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="time" />
                            </Col>
                        </Form.Group>
                        <Form.Group as={Row} className="mb-3">
                            <Form.Label column sm={3}>Hora de fin</Form.Label>
                            <Col sm={9}>
                                <Form.Control type="time" />
                            </Col>
                        </Form.Group>
                    </>
                );
            
            case "3": // Enclavamiento
                return (
                    <Button variant={relay.state ? "success" : "danger"} className="power-button rounded-circle" onClick={handleToggle}>
                        <FaPowerOff />
                    </Button>
                );
            case "4": // Activación por Eventos Externos
                return null; // No hay contenido para este modo
            default:
                return null;
        }
    };

    return (
        <Card>
            <Card.Body>
                <div className="d-flex justify-content-between align-items-center mb-3">
                    <Card.Title>{relay.name}</Card.Title>
                    {/* Botón de encendido/apagado */}
                    <Button variant={isOn ? 'success' : 'danger'} onClick={handleToggle} className='rounded-circle'>
                        <FaPowerOff />
                    </Button>
                </div>
                {/* Contenido del modo de funcionamiento */}
                {renderModeContent()}
            </Card.Body>
        </Card>
    );
};

export default RelayCard;
