import React from 'react';
import { Container, Row, Col } from 'react-bootstrap';
import RelayCard from './RelayCard';

const Panel = ({ Relay }) => {

    return (
        <div>
            <Row className="mb-3">
                <Col>
                    <h2>Panel</h2>
                </Col>
            </Row>
            <Container>
                <Row>
                    {/* Itera sobre las claves del objeto Relay */}
                    {Object.keys(Relay).map((key) => (
                        <Col md={6} key={key} className='mb-2'>
                            {/* Pasa cada objeto de relevo y su clave como prop a RelayCard */}
                            <RelayCard relay={Relay[key]} relayKey={key} />
                        </Col>
                    ))}
                </Row>
            </Container>
        </div>
    );
}

export default Panel;