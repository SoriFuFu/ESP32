import React, { useEffect, useState } from 'react';
import { Container, Card, Button, Modal, Form, Row, Col } from 'react-bootstrap';
import { MdTimer, MdTimerOff } from "react-icons/md";
import { FaPlay, FaPause, FaPowerOff, FaStop } from 'react-icons/fa';
import TimerModal from './TimerModal';

const Panel = ({ Relay }) => {
    const [webSocket, setWebSocket] = useState(null);
    const [showModal, setShowModal] = useState(false);
    const [activeRelay, setActiveRelay] = useState(null);
    const [relayStates, setRelayStates] = useState({
        K1: Relay.K1,
        K2: Relay.K2,
    });

    useEffect(() => {
        setRelayStates(prevState => ({
            ...prevState,
            K1: {
                ...prevState.K1,
                remainingTime: Relay.K1.active ? Relay.K1.remainingTime :  Relay.K1.timerSelected,
            },
            K2: {
                ...prevState.K2,
                remainingTime: Relay.K2.active ? Relay.K2.remainingTime :  Relay.K2.timerSelected,
            },

        }));
    }, []);
    

    useEffect(() => {
        // const ws = new WebSocket('ws://192.168.1.222:83');
        // const ws = new WebSocket('ws://bubela.duckdns.org:83');
        const ws = new WebSocket('ws://' + window.location.hostname + ':83');

        setWebSocket(ws);

        ws.onopen = () => {
            console.log('Conexión WebSocket abierta');
        };

        ws.onerror = (error) => {
            console.error('Error en WebSocket:', error);
        };

        ws.onclose = () => {
            console.log('Conexión WebSocket cerrada');
        };
    }, []);

    useEffect(() => {
        if (webSocket) {
            webSocket.onmessage = (event) => {
                const data = JSON.parse(event.data);
                if (data.action === 'UPDATE_TIMER') {
                    setRelayStates(prevState => ({
                        ...prevState,
                        K1: {
                            ...prevState.K1,
                            remainingTime: data.timeK1,
                        },
                        K2: {
                            ...prevState.K2,
                            remainingTime: data.timeK2,
                        },
                    }));
                } else if (data.relay in Relay) {

                    if (data.action === 'ON') {

                        setRelayIsOn(data.relay, true);
                        setRelayState(data.relay, 'ACTIVE');
                    }else if(data.action === 'OFF') {
                        setRelayIsOn(data.relay, false);
                        setRelayState(data.relay, 'OFF');
                    }else if(data.action === 'ACTIVE') {
                        setRelayState(data.relay, 'ACTIVE');
                    }else if(data.action === 'PAUSE') {
                        setRelayState(data.relay, 'PAUSE');
                    }else if(data.action === 'CONTINUE') {
                        setRelayState(data.relay, 'ACTIVE');
                    }else if(data.action === 'INACTIVE') {

                        setRelayState(data.relay, 'INACTIVE');
                    }
                }
            };
        }
    }, [webSocket]);
    const setRelayIsOn = (relay, value) => {
        setRelayStates(prevState => ({
            ...prevState,
            [relay]: {
                ...prevState[relay],
                isOn: value,
            },
        }));
    };
    const setRelayState = (relay, value) => {
 
        setRelayStates(prevState => ({
            ...prevState,
            [relay]: {
                ...prevState[relay],
                state: value,
            },
        }));
    };

    const setRelayTimerState = (relay, value) => {
        setRelayStates(prevState => ({
            ...prevState,
            [relay]: {
                ...prevState[relay],
                timerState: value,
            },
        }));
    };

    const handleRelayAction = (relay, action) => {

        if (relay in Relay) {
            if (action === 'ON') {
                setRelayIsOn(relay, true);
            } else if (action === 'OFF') {
                setRelayIsOn(relay, false);
            }

            if (webSocket && webSocket.readyState === WebSocket.OPEN) {
                const message = { relay: relay, action: action };
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
    
            let action;
            const relayData = relayStates[relay];
            console.log(relayData.state);
            if (relayData.state === 'ACTIVE') {
                setRelayState(relay, 'PAUSE'); // Actualizar el estado local del relé antes de enviar el mensaje WebSocket
                action = 'PAUSE';
            } else if (relayData.state === 'PAUSE') {
                setRelayState(relay, 'ACTIVE'); // Actualizar el estado local del relé antes de enviar el mensaje WebSocket
                action = 'CONTINUE';
            } else if (relayData.state === 'INACTIVE') {
                action = 'ACTIVE';
                setRelayState(relay, 'ACTIVE'); // Actualizar el estado local del relé antes de enviar el mensaje WebSocket
            }
    
            if (webSocket) {
                const message = { relay: relay, action: action, selectedTime: relayData.remainingTime };
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
                const message = { relay: relay, action: 'SET_TIMER', timerState: false };
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
        setRelayStates(prevState => ({
            ...prevState,
            [relay]: {
                ...prevState[relay],
                timerState: true,
                remainingTime: selectedTime
            }
        }));

        if (webSocket && webSocket.readyState === WebSocket.OPEN) {
            setShowModal(false);
            const message = { relay: relay, action: 'SET_TIMER', timerState: true, selectedTime: selectedTime };
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
                    {Object.keys(relayStates).map((relay) => (
                        <Col md={6} key={relay}>
                            {relayStates[relay].active && (
                                <Card className="mb-3">
                                    <Card.Body>
                                        <div className="d-flex justify-content-between align-items-center mb-3">
                                            <Card.Title>{relayStates[relay].name}</Card.Title>
                                            {!relayStates[relay].timerState && (
                                                <Button
                                                    variant={relayStates[relay].isOn ? 'success' : 'danger'}
                                                    onClick={() => handleRelayAction(relay, relayStates[relay].isOn ? 'OFF' : 'ON')}
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
                                                {relayStates[relay].timerState ? (
                                                    <>
                                                        <Col xs={6} md={6} className="text-center fs-3">
                                                            <div className="mx-3">
                                                                <span>{`${Math.floor(relayStates[relay].remainingTime / 3600000).toString().padStart(2, '0')}:${Math.floor((relayStates[relay].remainingTime % 3600000) / 60000).toString().padStart(2, '0')}:${Math.floor((relayStates[relay].remainingTime % 60000) / 1000).toString().padStart(2, '0')}`}</span>
                                                            </div>
                                                        </Col>
                                                        <Col xs={4} md={3} className="d-flex justify-content-right">
                                                            <Button variant="success" onClick={() => hundleRelayTimer(relay)}>
                                                                {renderIcon(relayStates[relay])}
                                                            </Button>
                                                            {relayStates[relay].state === 'ACTIVE' && (
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
