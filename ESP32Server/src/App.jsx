import React from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';
import { Container, Row, Col} from 'react-bootstrap';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import './App.css';
import Menu from './Menu';
import Panel from './Panel';
import Config from './Config';

const App = () => {
    return (
        <Router>
            <div>
                <Menu />
                <Container fluid className="content">
                <Routes>
                    <Route path="/panel" element={<Panel />} />
                    <Route path="/config" element={<Config />} />
                </Routes>
                </Container>
            </div>
        </Router>
    );
}

export default App;
