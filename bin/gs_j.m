clf; close all;

load('gs2_iter.txt')
load('j2_iter.txt')
load('gs1_time.txt')
load('j1_time.txt')

w = 300;
h = 240;
x = 1920 - w - 10;
y = 1080 - h - 90;

itsPlot = 1400;
timePlot = 4220;

% PLOT VS TIME
timesGS = gs1_time(1:timePlot, 1);
timesGS = timesGS - timesGS(1);
timesJ = j1_time(1:timePlot, 1);
timesJ = timesJ - timesJ(1);

set(0, 'DefaultFigurePosition', [x y w h]);
figure
tp = semilogy(timesGS, gs1_time(1:timePlot, 2), '-o', 'LineWidth', 2);
tp.MarkerSize = 8;
tp.MarkerIndices = 1:timePlot/5:timePlot;
hold on
tp = semilogy(timesJ, j1_time(1:timePlot, 2), '-s', 'LineWidth', 2);
tp.MarkerSize = 8;
tp.MarkerIndices = 1:timePlot/5:timePlot;
axis([0 10 0 10000])
xticks(0:2:10)
xlabel('Time [s]', 'Interpreter', 'LaTeX')
ylabel('Residual [log]', 'Interpreter', 'LaTeX')
legend({'Gauss-Seidel', 'Averaged Jacobi'}, 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')

% PLOT VS ITERATIONS
set(0, 'DefaultFigurePosition', [x y-h-84 w h]);
figure
ip = semilogy(gs2_iter(1:itsPlot, 2), '-o', 'LineWidth', 2);
ip.MarkerSize = 8;
ip.MarkerIndices = 1:itsPlot/5:itsPlot;
hold on
ip = semilogy(j2_iter(1:itsPlot, 2), '-s', 'LineWidth', 2);
ip.MarkerSize = 8;
ip.MarkerIndices = 1:itsPlot/5:itsPlot;

axis([0 itsPlot 0 10^4])
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Residual [log]', 'Interpreter', 'LaTeX')
legend({'Gauss-Seidel', 'Averaged Jacobi'}, 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')