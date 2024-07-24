import React, { useEffect, useState } from 'react';
import { Container, Card, Button, Modal, Form, Row, Col } from 'react-bootstrap';
import { MdTimer, MdTimerOff } from "react-icons/md";
import { FaPlay, FaPause, FaPowerOff, FaStop } from 'react-icons/fa';
import TimerModal from './TimerModal';

const Panel = ({ webSocket, Relay, updateRelayApp }) => {
    
    const [showModal, setShowModal] = useState(false);
    const [activeRelay, setActiveRelay] = useState(null);


    useEffect(() => {
        updateRelayApp('K1', "remainingTime", Relay.K1.active ? Relay.K1.remainingTime :  Relay.K1.timerSelected);
        updateRelayApp('K2', "remainingTime", Relay.K2.active ? Relay.K2.remainingTime :  Relay.K2.timerSelected);
    }, []);
    
    
    const setRelayIsOn = (relay, value) => {
        updateRelayApp(relay, 'isOn', value);
    };
    const setRelayState = (relay, value) => {
        updateRelayApp(relay, 'state', value);
    };

    const setRelayTimerState = (relay, value) => {
        updateRelayApp(relay, 'timerState', value);
    };

    const handleRelayAction = (relay, command) => {

        if (relay in Relay) {
            if (command === 'ON') {
                setRelayIsOn(relay, true);
            } else if (command === 'OFF') {
                setRelayIsOn(relay, false);
            }else if (command === 'INACTIVE') {
                setRelayState(relay, 'INACTIVE');
            }

            if (webSocket && webSocket.readyState === WebSocket.OPEN) {
                const message = { action: "RELAYHANDLER", relay: relay, command: command };
                webSocket.send(JSON.stringify(message));
            } else {
                console.log('WebSocket no está completamente abierto');
            }
        }
    };


    // FUNCION PARA CAMBIAR EL ESTADO DEL RELÉ CON TIMER
    const hundleRelayTimer = (relay) => {
        console.log(relay);
        if (relay in Relay) {
            let command;
            const relayData = Relay[relay];
            if (relayData.state === 'ACTIVE') {
                // setRelayState(relay, 'PAUSE'); // Actualizar el estado local del relé antes de enviar el mensaje WebSocket
                command = 'PAUSE';
            } else if (relayData.state === 'PAUSE') {
                // setRelayState(relay, 'ACTIVE'); // Actualizar el estado local del relé antes de enviar el mensaje WebSocket
                command = 'CONTINUE';
            } else if (relayData.state === 'INACTIVE') {
                command = 'ACTIVE';
                // setRelayState(relay, 'ACTIVE'); // Actualizar el estado local del relé antes de enviar el mensaje WebSocket
            }
    
            if (webSocket) {
                const message = {action: "RELAYHANDLER", relay: relay, command: command, selectedTime: relayData.remainingTime };
                webSocket.send(JSON.stringify(message));
            } else {
                console.log('WebSocket no está inicializado');
            }
        }
    };
    


    // FUNCION PARA APAGAR EL TIMER
    const handleTimerOff = (relay, value) => {
        console.log(relay, value);
        if (relay in Relay) {
            if (webSocket) {
                const message = {action: "RELAYHANDLER", relay: relay, command: 'SET_TIMER', timerState: false };
                webSocket.send(JSON.stringify(message));
            } else {
                console.log('WebSocket no está inicializado');
            }
            setRelayTimerState(relay, value);
        }
        handleModalClose();
    };

    // FUNCIONES PARA EL MODAL
    const handleModalClose = () => {
        setShowModal(false);
    };

    const handleModalShow = (relay) => {
        setActiveRelay(relay);
        setShowModal(true);
    };

    // FUNCION PARA SELECCIONAR EL TIEMPO
    const handleTimeSelection = (relay, selectedTime) => {
        updateRelayApp(relay, 'timerState', true);
        updateRelayApp(relay, 'remainingTime', selectedTime);

        if (webSocket && webSocket.readyState === WebSocket.OPEN) {
            setShowModal(false);
            const message = {action: "RELAYHANDLER", relay: relay, command: 'SET_TIMER', timerState: true, selectedTime: selectedTime };
            webSocket.send(JSON.stringify(message));
        } else {
            console.log('WebSocket no está inicializado');
        }
    };

    const renderIcon = (relayState) => {
        return relayState.state === 'ACTIVE' ? <FaPause /> : <FaPlay />;
    };


    return (
        <div>
            <Row className="mb-3">
                <Col>
                    <h2>Panel</h2>
                </Col>
            </Row>
            <Container>
                <Row>
                    {Object.keys(Relay).map((relay) => (
                        <Col md={6} key={relay}>
                            {Relay[relay].active && (
                                <Card className="mb-3">
                                    <Card.Body>
                                        <div className="d-flex justify-content-between align-items-center mb-3">
                                            <Card.Title>{Relay[relay].name}</Card.Title>
                                            {!Relay[relay].timerState && (
                                                <Button
                                                    variant={Relay[relay].isOn ? 'success' : 'danger'}
                                                    onClick={() => handleRelayAction(relay, Relay[relay].isOn ? 'OFF' : 'ON')}
                                                    className='d-flex justify-content-center align-items-center rounded-circle'
                                                    style={{ width: '40px', height: '40px' }}
                                                >
                                                    <FaPowerOff />
                                                </Button>
                                            )}
                                        </div>
                                        <>
                                            <Row className="d-flex justify-content-between align-items-center mt-3">
                                                <Col xs={2} md={3}>
                                                    <Button variant="warning" onClick={() => handleModalShow(relay)}>
                                                        {Relay[relay].timerState ? <MdTimer /> : <MdTimerOff />}
                                                    </Button>
                                                </Col>
                                                {Relay[relay].timerState ? (
                                                    <>
                                                        <Col xs={6} md={6} className="text-center fs-3">
                                                            <div className="mx-3">
                                                                <span>{`${Math.floor(Relay[relay].remainingTime / 3600000).toString().padStart(2, '0')}:${Math.floor((Relay[relay].remainingTime % 3600000) / 60000).toString().padStart(2, '0')}:${Math.floor((Relay[relay].remainingTime % 60000) / 1000).toString().padStart(2, '0')}`}</span>
                                                            </div>
                                                        </Col>
                                                        <Col xs={4} md={3} className="d-flex justify-content-right">
                                                            <Button variant="success" onClick={() => hundleRelayTimer(relay)}>
                                                                {renderIcon(Relay[relay])}
                                                            </Button>
                                                            {Relay[relay].state === 'ACTIVE' && (
                                                                <Button variant="danger" onClick={() => handleRelayAction(relay, 'INACTIVE')} className='d-flex justify-content-center align-items-center ms-2' style={{ width: '40px', height: '40px' }}>
                                                                    <FaStop />
                                                                </Button>
                                                            )}
                                                        </Col>
                                                    </>
                                                ) : (
                                                    <></>
                                                )}
                                            </Row>

                                        </>
                                    </Card.Body>
                                </Card>
                            )}
                        </Col>
                    ))}
                </Row>
            </Container>
            {showModal && activeRelay && (
                <TimerModal
                    relay={activeRelay}
                    relayState={Relay[activeRelay]}
                    handleModalClose={handleModalClose}
                    handleTimeSelection={handleTimeSelection}
                    handleTimerOff={handleTimerOff}
                />
            )}
        </div>
    );

}

export default Panel;
