function varargout = simple_gui(varargin)
% SIMPLE_GUI MATLAB code for simple_gui.fig
%      SIMPLE_GUI, by itself, creates a new SIMPLE_GUI or raises the existing
%      singleton*.
%
%      H = SIMPLE_GUI returns the handle to a new SIMPLE_GUI or the handle to
%      the existing singleton*.
%
%      SIMPLE_GUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SIMPLE_GUI.M with the given input arguments.
%
%      SIMPLE_GUI('Property','Value',...) creates a new SIMPLE_GUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before simple_gui_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to simple_gui_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help simple_gui

% Last Modified by GUIDE v2.5 20-Mar-2014 02:40:53

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @simple_gui_OpeningFcn, ...
                   'gui_OutputFcn',  @simple_gui_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before simple_gui is made visible.
function simple_gui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to simple_gui (see VARARGIN)
h2=handles.axes2;
axes(h2);
%title('accelerometer');
xlabel('time (s)');
ylabel('altitude (cm)');

h3=handles.axes3;
axes(h3);
%title('accelerometer');
%legend('ax','ay','az');
xlabel('time (s)');
ylabel('acceleration (g)');

h4=handles.axes4;
axes(h4);
%title('gyro')
%legend('gx','gy','gz');
xlabel('time (s)');
ylabel('angular velocity (deg/sec)');

h5=handles.axes5;
axes(h5);
%title('gyro')
%legend('gx','gy','gz');
xlabel('time (s)');
ylabel('servo input (ms)');

h6=handles.axes6;
axes(h6);
%title('gyro');
%legend('gx','gy','gz');
xlabel('time (s)');
ylabel('esc output (ms)');

% Choose default command line output for simple_gui
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes simple_gui wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = simple_gui_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = cellstr(get(hObject,'String')) returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1


% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in start.
function start_Callback(hObject, eventdata, handles)

% hObject    handle to start (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in stop.
function stop_Callback(hObject, eventdata, handles)

% hObject    handle to stop (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


% --- Executes on button press in togglebutton1.
function togglebutton1_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton1
if get(hObject, 'Value')
    inst = instrfind;
    if length(inst) > 0 
        fclose(instrfind);
    end
    s = serial('COM2');
    s.BaudRate = 115200;
    s.Terminator = '';
    s.InputBufferSize = 32768;
    if s.Status == 'closed'
        fopen(s);
    end
    disp('serial on');
    
    % Initialize axis variables
    tic
    tmr = toc;

    maxlen = 500;
    t = zeros(1,maxlen);
    ax = zeros(1,maxlen);
    ay = zeros(1,maxlen);
    az = zeros(1,maxlen);
    gx = zeros(1,maxlen);
    gy = zeros(1,maxlen);
    gz = zeros(1,maxlen);
    alt = zeros(1,maxlen);
    c1 = zeros(1,maxlen);
    c2 = zeros(1,maxlen);
    c3 = zeros(1,maxlen);
    c4 = zeros(1,maxlen);
    m1 = zeros(1,maxlen);
    m2 = zeros(1,maxlen);
    m3 = zeros(1,maxlen);
    m4 = zeros(1,maxlen);
    
    global m1_cmd;
    global m2_cmd;
    global m3_cmd;
    global m4_cmd;
    

    % Add legends and axes labels
    h2=handles.axes2;
    axes(h2);
    ah2 = plot(t,alt);
    %title('accelerometer');
    xlabel('time (s)');
    ylabel('altitude (cm)');
    
    h3=handles.axes3;
    axes(h3);
    ah3 = plot(t,ax,t,ay,t,az);
    %title('accelerometer');
    %legend('ax','ay','az');
    xlabel('time (s)');
    ylabel('acceleration (g)');

    h4=handles.axes4;
    axes(h4);
    ah4 = plot(t,gx,t,gy,t,gz);
    %title('gyro');
    %legend('gx','gy','gz');
    xlabel('time (s)');
    ylabel('angular velocity (deg/sec)');
    
    h5=handles.axes5;
    axes(h5);
    ah5 = plot(t,c1,t,c2,t,c3,t,c4);
    %title('gyro');
    %legend('gx','gy','gz');
    xlabel('time (s)');
    ylabel('servo input (ms)');
    
    h6=handles.axes6;
    axes(h6);
    ah6 = plot(t,m1,t,m2,t,m3,t,m4);
    %title('gyro');
    %legend('gx','gy','gz');
    xlabel('time (s)');
    ylabel('esc output (ms)');

    t1 = toc;
    dt = 0;

    time = 0;
    for i = 1:50
        tic; fread(s,68); time = time + toc;
    end
    time = time/50;

    while get(hObject, 'Value')
        bytes = s.BytesAvailable;
        %fprintf(s,'q?'); 
        out = zeros(1,34);

        if bytes > 68
            data = fread(s,bytes);
            for i = length(data)-33:-1:length(data)-67
                if data(i) == 115
                    out = data(i:i+33);
                    break;
                end
            end
        end

        % check that string starts with s and ends with e
        if out(1) == 115 && out(end) == 101
            %disp(out)
            t(1:end-1) = t(2:end);
            t(end) = toc;
            ax(1:end-1) = ax(2:end);
            if out(2) < 128 
                ax(end) = out(3)+out(2)*256;
            else
                ax(end) = -1*(out(3)+(out(2)-128)*256);
            end
            ay(1:end-1) = ay(2:end);
            if out(4) < 128 
                ay(end) = out(5)+out(4)*256;
            else
                ay(end) = -1*(out(5)+(out(4)-128)*256);
            end
            az(1:end-1) = az(2:end);
            if out(6) < 128 
                az(end) = out(7)+out(6)*256;
            else
                az(end) = -1*(out(7)+(out(6)-128)*256);
            end

            gx(1:end-1) = gx(2:end);
            if out(8) < 128 
                gx(end) = out(9)+out(8)*256;
            else
                gx(end) = -1*(out(9)+(out(8)-128)*256);
            end
            gy(1:end-1) = gy(2:end);
            if out(10) < 128 
                gy(end) = out(11)+out(10)*256;
            else
                gy(end) = -1*(out(11)+(out(10)-128)*256);
            end
            gz(1:end-1) = gz(2:end);
            if out(12) < 128 
                gz(end) = out(13)+out(12)*256;
            else
                gz(end) = -1*(out(13)+(out(12)-128)*256);
            end
            alt(1:end-1) = alt(2:end);
            if out(14) < 128 
                alt(end) = out(15)+out(14)*256;
            else
                alt(end) = -1*(out(15)+(out(14)-128)*256);
            end
            c1(1:end-1) = c1(2:end);
            if out(16) < 128 
                c1(end) = out(17)+out(16)*256;
            else
                c1(end) = -1*(out(17)+(out(16)-128)*256);
            end
            c2(1:end-1) = c2(2:end);
            if out(18) < 128 
                c2(end) = out(19)+out(18)*256;
            else
                c2(end) = -1*(out(19)+(out(18)-128)*256);
            end
            c3(1:end-1) = c3(2:end);
            if out(20) < 128 
                c3(end) = out(21)+out(20)*256;
            else
                c3(end) = -1*(out(21)+(out(20)-128)*256);
            end
            c4(1:end-1) = c4(2:end);
            if out(22) < 128 
                c4(end) = out(23)+out(22)*256;
            else
                c4(end) = -1*(out(23)+(out(22)-128)*256);
            end
            m1(1:end-1) = m1(2:end);
            if out(24) < 128 
                m1(end) = out(25)+out(24)*256;
            else
                m1(end) = -1*(out(25)+(out(24)-128)*256);
            end
            m2(1:end-1) = m2(2:end);
            if out(26) < 128 
                m2(end) = out(27)+out(26)*256;
            else
                m2(end) = -1*(out(27)+(out(26)-128)*256);
            end
            m3(1:end-1) = m3(2:end);
            if out(28) < 128 
                m3(end) = out(29)+out(28)*256;
            else
                m3(end) = -1*(out(29)+(out(28)-128)*256);
            end
            m4(1:end-1) = m4(2:end);
            if out(30) < 128 
                m4(end) = out(31)+out(30)*256;
            else
                m4(end) = -1*(out(31)+(out(30)-128)*256);
            end
            
            % Only plot at 50hz
            if toc-tmr > .015 
                axes(h2);
                set(ah2(1),'XData',t,'YData',alt);
                xlim([min(t),max(t)]);
                
                axes(h3);
                set(ah3(1),'XData',t,'YData',ax);
                set(ah3(2),'XData',t,'YData',ay);
                set(ah3(3),'XData',t,'YData',az);
                xlim([min(t),max(t)]);

                axes(h4);
                set(ah4(1),'XData',t,'YData',gx);
                set(ah4(2),'XData',t,'YData',gy);
                set(ah4(3),'XData',t,'YData',gz);
                xlim([min(t),max(t)]);
                
                axes(h5);
                set(ah5(1),'XData',t,'YData',c1);
                set(ah5(2),'XData',t,'YData',c2);
                set(ah5(3),'XData',t,'YData',c3);
                set(ah5(4),'XData',t,'YData',c4);
                xlim([min(t),max(t)]);
                
                axes(h6);
                set(ah6(1),'XData',t,'YData',m1);
                set(ah6(2),'XData',t,'YData',m2);
                set(ah6(3),'XData',t,'YData',m3);
                set(ah6(4),'XData',t,'YData',m4);
                xlim([min(t),max(t)]);

                tmr = toc;
            end
        end
        dt = toc-t1;
        t1 = toc;
        %sdisp(dt);
        
        
        if length(s.Status) == 4
            fprintf(s,'a');
            fprintf(s,num2str(m1_cmd));
            fprintf(s,'a');
            fprintf(s,'b');
            fprintf(s,num2str(m2_cmd));
            fprintf(s,'b');
            fprintf(s,'c');
            fprintf(s,num2str(m3_cmd));
            fprintf(s,'c');
            fprintf(s,'d');
            fprintf(s,num2str(m4_cmd));
            fprintf(s,'d');
        end
        pause(.01)
    end
else
    inst = instrfind;
    if length(inst) > 0 
        fclose(instrfind);
    end
    disp('serial off');
end



% --- Executes on button press in togglebutton2.
function togglebutton2_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton2


% --- Executes on button press in togglebutton3.
function togglebutton3_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Open COM Port

% Hint: get(hObject,'Value') returns toggle state of togglebutton3



function edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double
global m1_cmd;
user_entry = str2double(get(hObject,'string'));
if isnan(user_entry)
    errordlg('You must enter a numeric value','Bad Input','modal')
    uicontrol(hObject)
	return
end
if user_entry > 1000 || user_entry < 0
    errordlg('Input must be < 1000 and > 0');
    uicontrol(hObject)
	return
end
m1_cmd = user_entry;

% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double
global m2_cmd;
user_entry = str2double(get(hObject,'string'));
if isnan(user_entry)
    errordlg('You must enter a numeric value','Bad Input','modal')
    uicontrol(hObject)
	return
end
if user_entry > 1000 || user_entry < 0
    errordlg('Input must be < 1000 and > 0');
    uicontrol(hObject)
	return
end
m2_cmd = user_entry;

% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit4_Callback(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit4 as text
%        str2double(get(hObject,'String')) returns contents of edit4 as a double
global m3_cmd;
user_entry = str2double(get(hObject,'string'));
if isnan(user_entry)
    errordlg('You must enter a numeric value','Bad Input','modal')
    uicontrol(hObject)
	return
end
if user_entry > 1000 || user_entry < 0
    errordlg('Input must be < 1000 and > 0');
    uicontrol(hObject)
	return
end
m3_cmd = user_entry;

% --- Executes during object creation, after setting all properties.
function edit4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit5_Callback(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit5 as text
%        str2double(get(hObject,'String')) returns contents of edit5 as a double
global m4_cmd;
user_entry = str2double(get(hObject,'string'));
if isnan(user_entry)
    errordlg('You must enter a numeric value','Bad Input','modal')
    uicontrol(hObject)
	return
end
if user_entry > 1000 || user_entry < 0
    errordlg('Input must be < 1000 and > 0');
    uicontrol(hObject)
	return
end
m4_cmd = user_entry;



% --- Executes during object creation, after setting all properties.
function edit5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in togglebutton4.
function togglebutton4_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton4
