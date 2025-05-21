export interface User {
  id: string;
  name: string;
  role: 'student' | 'staff' | 'admin';
  rfidId: string;
}

export interface Lab {
  id: string;
  name: string;
  location: string;
  capacity: number;
}

export interface RFIDReader {
  id: string;
  labId: string;
  status: 'online' | 'offline' | 'error';
  lastPing: Date;
}

export interface ScanLog {
  id: string;
  userId: string;
  userName: string;
  userRole: 'student' | 'staff' | 'admin';
  rfidId: string;
  readerId: string;
  labId: string;
  labName: string;
  timestamp: Date;
  direction: 'in' | 'out';
}

export interface ServerStats {
  totalScansToday: number;
  activeUsers: number;
  totalUsers: number;
  lastScanTime: Date | null;
}