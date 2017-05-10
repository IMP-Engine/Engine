clf; close all;

load('gsPara1.txt')
load('gsPara2.txt')
load('gsPara3.txt')
load('gsPara4.txt')
load('gsPara5.txt')
load('gsPara6.txt')
load('gsPara7.txt')
load('gsPara8.txt')

maxTime = 10;
maxRes = 10^4;
timePlot = 8600;

time1 = gsPara1(1:timePlot, 1) - gsPara1(1, 1);
time2 = gsPara2(1:timePlot, 1) - gsPara2(1, 1);
time3 = gsPara3(1:timePlot, 1) - gsPara3(1, 1);
time4 = gsPara4(1:timePlot, 1) - gsPara4(1, 1);
time5 = gsPara5(1:timePlot, 1) - gsPara5(1, 1);
time6 = gsPara6(1:timePlot, 1) - gsPara6(1, 1);
time7 = gsPara7(1:timePlot, 1) - gsPara7(1, 1);
time8 = gsPara8(1:timePlot, 1) - gsPara8(1, 1);
%times = [time1 time2 time3 time4 time5 time6 time7 time8];
times = [time1 time2 time4 time8];

res1 = gsPara1(1:timePlot, 3);
res2 = gsPara2(1:timePlot, 3);
res3 = gsPara3(1:timePlot, 3);
res4 = gsPara4(1:timePlot, 3);
res5 = gsPara5(1:timePlot, 3);
res6 = gsPara6(1:timePlot, 3);
res7 = gsPara7(1:timePlot, 3);
res8 = gsPara8(1:timePlot, 3);
%res = [res1 res2 res3 res4 res5 res6 res7 res8];
res = [res1 res2 res4 res8];

w = 300;
h = 240;
x = 1920 - w - 10;
y = 1080 - h - 90;

markers = '^vos*xsd^v';
markerSize = 10;

% PLOT VS TIME
set(0, 'DefaultFigurePosition', [x y w h]);
figure
for i = 1 : size(res, 2)
	tp = semilogy(times(:, i), res(:, i), strcat('-', markers(i)), 'LineWidth', 2);
	tp.MarkerSize = markerSize;
	tp.MarkerIndices = 1000 + 900*(i-1);%1:timePlot/5:timePlot;
	hold on
end

axis([0 maxTime 0 maxRes])
xticks(0:maxTime/4:maxTime)
xlabel('Time [s]', 'Interpreter', 'LaTeX')
ylabel('Residual [log]', 'Interpreter', 'LaTeX')
%legend({'1', '2', '3', '4', '5', '6', '7', '8'}, 'Interpreter', 'LaTeX')
legend({'1', '2', '4', '8'}, 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')

% PLOT VS ITERATIONS
set(0, 'DefaultFigurePosition', [x y-h-84 w h]);
figure
for i = 1 : size(res, 2)
	ip = semilogy(res(:, i), strcat('-', markers(i)), 'LineWidth', 2);
	ip.MarkerSize = markerSize;
	%ip.MarkerIndices = 400*i : floor(timePlot/(1+i*0.25)) : timePlot;
	ip.MarkerIndices = 4000 + 1000*(i-1);
	hold on
end

axis([0 timePlot 0 10^4])
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Residual [log]', 'Interpreter', 'LaTeX')
%legend({'1', '2', '3', '4', '5', '6', '7', '8'}, 'Interpreter', 'LaTeX')
legend({'1', '2', '4', '8'}, 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')