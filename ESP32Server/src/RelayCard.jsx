import React, { useEffect, useState } from 'react';
import { Card, Button, Modal, Form, Row, Col } from 'react-bootstrap';
import { MdTimer, MdTimerOff } from "react-icons/md";
import { FaPlay, FaPause, FaPowerOff, FaStop } from 'react-icons/fa';


const RelayCard = ({ relay, relayKey, webSocket, updatedTimes  }) => {
    const [isOn, setIsOn] = useState(false);
    const [remainingTime, setRemainingTime] = useState(relay.timer);
    const [relayState, setRelayState] = useState(relay.state);
    const [showModal, setShowModal] = useState(false);
    const [selectedTime, setSelectedTime] = useState(relay.timer);
    const [timerStatus, setTimerStatus] = useState(relay.timerStatus);
    console.log(updatedTimes)
    useEffect(() => {
        if (updatedTimes && updatedTimes[`timeK${relayKey}`] !== undefined) {
            setRemainingTime(updatedTimes[`timeK${relayKey}`]);
        }
    }, [updatedTimes, relayKey]);

    useEffect(() => {
        if (webSocket) {
            webSocket.onmessage = (event) => {
                const data = JSON.parse(event.data);
                if (data.relay === relayKey) {
                     if (data.action === 'UPDATE_TIMER_STATUS') {
                        setTimerStatus(data.status);
                    } else if (data.action === 'ON') {
                        setIsOn(true);
                        setRelayState('ON');
                    } else if (data.action === 'ACTIVE') {
                        setRelayState('ACTIVE');
                    } else if (data.action === 'PAUSE') {
                        setRelayState('PAUSE');
                    } else if (data.action === 'CONTINUE') {
                        setRelayState('ACTIVE');
                    } else if (data.action === 'INACTIVE') {
                        setRelayState('INACTIVE');
                    } else if (data.action === 'OFF') {
                        setIsOn(false);
                        setRelayState('OFF');
                    }
                }
            };
        }
    }, [webSocket]);
    useEffect(() => {

        if (remainingTime === 0) {
            setRelayState('INACTIVE');
            setIsOn(false);
            setRemainingTime(selectedTime);
        }
    }, [remainingTime]);

    // FUNCION PARA CAMBIAR EL ESTADO DEL RELÉ SIN TIMER
    const hundleRelayChange = (action) => {
        if (action === 'ON') {
            setIsOn(true);
        }
        if (action === 'OFF') {
            setIsOn(false);
        }

        if (webSocket && webSocket.readyState === WebSocket.OPEN) {
            let message = { relay: relayKey, action: action };
            webSocket.send(JSON.stringify(message));
        } else {
            console.log('WebSocket no está completamente abierto');
        }
    };

    // FUNCION PARA CAMBIAR EL ESTADO DEL RELÉ CON TIMER
    const hundleRelayTimer = () => {
        let action;
        if (relayState === 'ACTIVE') {
            setRelayState('PAUSE');
            action = 'PAUSE';
        } else if (relayState === 'PAUSE') {
            setRelayState('ACTIVE');
            action = 'CONTINUE';
        } else if (relayState === 'INACTIVE') {
            setRelayState('ACTIVE');
            action = 'ACTIVE';
            setIsOn(true);
        }
        if (webSocket) {
            let message = { relay: relayKey, action: action, selectedTime: remainingTime };
            webSocket.send(JSON.stringify(message));
        }
        else {
            console.log('WebSocket no está inicializado');
        }
    };

    const renderIcon = () => {
        return relayState === 'ACTIVE' ? <FaPause /> : <FaPlay />;
    };

    // FUNCIONES PARA EL MODAL
    const handleModalClose = () => {
        setShowModal(false);
    };

    const handleModalShow = () => {
        setShowModal(true);
    };

    // FUNCION PARA SELECCIONAR EL TIEMPO
    const handleTimeSelection = () => {
        setTimerStatus(true);
        if (webSocket && webSocket.readyState === WebSocket.OPEN) {
            setRemainingTime(selectedTime);
            setShowModal(false);
            setRelayState('INACTIVE');
            let message = { relay: relayKey, action: 'SET_TIMER', timerStatus: true, selectedTime: selectedTime };
            webSocket.send(JSON.stringify(message));
        } else {
            console.log('WebSocket no está inicializado');
        }

    };

    const handleTimerOff = () => {
        setTimerStatus(false);
        if (webSocket && webSocket.readyState === WebSocket.OPEN) {
            let message = { relay: relayKey, action: 'SET_TIMER', timerStatus: false, selectedTime: selectedTime };
            webSocket.send(JSON.stringify(message));
            setShowModal(false);
        } else {
            console.log('WebSocket no está inicializado');
        }
    }

    return (
        <Card>
            <Card.Body>
                <div className="d-flex justify-content-between align-items-center mb-3">
                    <Card.Title>{relay.name}</Card.Title>
                    {!timerStatus && (<Button
                        variant={isOn ? 'success' : 'danger'}
                        onClick={() => hundleRelayChange(isOn ? 'OFF' : 'ON')}
                        className='d-flex justify-content-center align-items-center rounded-circle'
                        style={{ width: '40px', height: '40px' }}
                    >
                        <FaPowerOff />
                    </Button>)}
                </div>

                <>
                    <Row className="d-flex justify-content-between align-items-center mt-3">
                        {/* Botón de temporizador */}
                        <Col xs={3} md={3} >
                            <Button variant="warning" onClick={handleModalShow} >{timerStatus ? (<MdTimer />) : (<MdTimerOff />)}</Button>
                        </Col>
                        {timerStatus ? (
                            <>
                                {/* Contador */}
                                <Col xs={6} md={6} className="text-center fs-3">
                                    <div className="mx-3">
                                        <span>{`${Math.floor(remainingTime / 3600000).toString().padStart(2, '0')}:${Math.floor((remainingTime % 3600000) / 60000).toString().padStart(2, '0')}:${Math.floor((remainingTime % 60000) / 1000).toString().padStart(2, '0')}`}</span>
                                    </div>
                                </Col>
                                {/* Botón de encendido/apagado */}
                                <Col xs={3} md={3} className="d-flex justify-content-right" >
                                    <Button variant="success"
                                        onClick={() => { hundleRelayTimer() }}
                                    >
                                        {renderIcon()}
                                    </Button>
                                    {relayState === 'ACTIVE' && (
                                        <Button variant="danger" onClick={() => hundleRelayChange('INACTIVE')} className='d-flex justify-content-center align-items-center ms-2' style={{ width: '40px', height: '40px' }}>
                                            <FaStop />
                                        </Button>
                                    )}
                                </Col>
                            </>

                        ) : (<>

                        </>)
                        }
                    </Row>
                    {/* Modal para seleccionar el tiempo */}
                    <Modal show={showModal} onHide={handleModalClose}>
                        <Modal.Header closeButton>
                            <Modal.Title>Seleccionar Tiempo</Modal.Title>
                        </Modal.Header>
                        <Modal.Body>
                            <Form>
                                <Row className="align-items-center">
                                    <Col xs={9} sm={9}>
                                        <Form.Control
                                            type="range"
                                            className='form-range'
                                            value={selectedTime}
                                            min={0}
                                            max={7200000} // Máximo de 2 horas en milisegundos
                                            step={60000} // Pasos de 1 segundo
                                            onChange={(e) => setSelectedTime(parseInt(e.target.value))}
                                        />
                                    </Col>
                                    <Col xs={3} sm={3}>
                                        <Form.Control
                                            type="text"
                                            readOnly
                                            value={`${Math.floor(selectedTime / 3600000).toString().padStart(2, '0')}:${Math.floor((selectedTime % 3600000) / 60000).toString().padStart(2, '0')}`}
                                        />
                                    </Col>

                                </Row>
                            </Form>
                        </Modal.Body>
                        <Modal.Footer>
                            {timerStatus && (
                                <Button variant="secondary" onClick={handleTimerOff}>
                                    Desactivar
                                </Button>
                            )}
                            <Button variant="secondary" onClick={handleModalClose}>
                                Cancelar
                            </Button>
                            <Button variant="primary" onClick={handleTimeSelection}>
                                Hecho
                            </Button>
                        </Modal.Footer>
                    </Modal>
                </>

            </Card.Body>
        </Card>
    );
};

export default RelayCard;