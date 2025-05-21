import React from 'react';
import { AppProvider } from './context/AppContext';
import Header from './components/Header';
import Dashboard from './components/Dashboard';
import LogsTable from './components/LogsTable';
import RFIDSimulator from './components/RFIDSimulator';
import DataControls from './components/DataControls';
import Tabs from './components/Tabs';

function App() {
  return (
    <AppProvider>
      <div className="min-h-screen bg-gray-100">
        <Header />
        
        <main className="container mx-auto">
          <Dashboard />
          
          <Tabs labels={['Attendance Logs', 'RFID Simulator', 'Data Management']}>
            <LogsTable />
            <RFIDSimulator />
            <DataControls />
          </Tabs>
        </main>
        
        <footer className="bg-gray-800 text-gray-300 py-6 mt-10">
          <div className="container mx-auto px-6">
            <div className="flex flex-col md:flex-row justify-between items-center">
              <div className="mb-4 md:mb-0">
                <h3 className="text-lg font-semibold">RFID Lab Attendance System</h3>
                <p className="text-sm text-gray-400">Secure, real-time tracking for lab access</p>
              </div>
              
              <div className="text-sm">
                <p>&copy; {new Date().getFullYear()} University Computer & Biomedical Labs</p>
                <p className="text-gray-500">Version 1.0.0</p>
              </div>
            </div>
          </div>
        </footer>
      </div>
    </AppProvider>
  );
}

export default App;