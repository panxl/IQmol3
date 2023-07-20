#pragma once
/*******************************************************************************

  Copyright (C) 2022 Andrew Gilbert

  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.

  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software
  Foundation, either version 3 of the License, or (at your option) any later
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.  
   
********************************************************************************/

#include "Timer.h"
#include "JobInfo.h"


class QProcess;

namespace IQmol {
namespace Process {

   class JobMonitor;

   /// The Job class augments the JobInfo class with signals, slots and timers.
   class Job : public QObject, public JobInfo {

      Q_OBJECT

      friend class JobMonitor;
      friend class Server;

      public:
		 /// This is the message that is displayed in the status
		 /// column of the JobMonitor.
         QString const& message() const { return m_message; }

         //void setMessage(QString const& message) { m_message = message; }

         JobInfo::Status status() const { return jobStatus(); }
         bool isActive() const { return JobInfo::isActive(jobStatus()); }

         /// This is an external handle for the process, either a
         /// PID or PBS/SGE job number.
         QString jobId()      const { return get<QString>("JobId"); }
         QString jobName()    const { return get<QString>("JobName"); }
         QString serverName() const { return get<QString>("ServerName"); }
         QString submitTime() const { return get<QString>("SubmitTime"); }
         unsigned julianDay() const { return get<unsigned>("JulianDay"); }
  
         bool localFilesExist() const;
         void setSubmitTime(QString const& string) { m_submitTime = string; }


         /// The run time in seconds.  Use Util::Timer::formatTime() 
         /// to turn it into hh:mm:ss
         unsigned runTime() const;

         /// This function substitutes the ${VARIABLES} with the values
         /// appropriate for the job.
         QString substituteMacros(QString const& string) const;

         /// This function needs to parse whatever is returned by the 
         /// query command and update the status accordingly
         virtual void parseQueryOutput(QString const&);

//         JobInfo& jobInfo() { return m_jobInfo; }

      public Q_SLOTS:
         //void copyProgress();
         void copyProgress(double);

      Q_SIGNALS:
         void updated();
         void finished();
         void error();
         void deleted(Job*);

      protected:
         /// Job construction should only be done via the JobMonitor, 
         /// hence we protect the constructor and destructor.
         Job() { }
         Job(JobInfo const&);

		 /// Note that deleting a Job will not result in the termination 
		 /// of the process.  This allows jobs to continue running even 
         /// after IQmol has terminated.
         ~Job();

         void setStatus(JobInfo::Status const status, QString const& message = QString());

		 /// Converts the job information into a form suitable 
		 /// for writing to the Preferences...
         virtual QVariant toQVariant() const;

         /// ...and back again
         bool fromQVariant(QVariant const&);



         //void setJobName(QString const& jobName) { m_jobName = jobName; }
         //void setServerName(QString const& serverName) { m_serverName = serverName; }

//         void setJobId(QString const& id) { m_jobId = id; }

		 /// The Job timer runs independantly of the actual process and so
		 /// may get out of sync if, for example, the process is suspended. 
         /// This function can be used to set the timer to the actual run 
		 /// time when it is known.
         void resetTimer(unsigned const seconds);

         QString const& copyProgressString() const { return m_copyProgress; }

      private:

         QString  m_jobName;
         QString  m_serverName;
         QString  m_message;
         QString  m_submitTime;
         QString  m_jobId;
//         qint64   m_julianDay;
//         bool     m_localFilesExist;
//         QString  m_localWorkingDirectory;



         Util::Timer m_timer;
         QString m_copyProgress;
   };

   typedef QList<Job*> JobList;

} } // end namespaces IQmol::Process
