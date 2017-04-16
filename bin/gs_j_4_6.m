%% Gauss-Seidel
clf; close all;
load('gs4.txt')
load('gs5.txt')
load('gs6.txt')

itsPlot = 1200;

iter = gs4(1:itsPlot, 1);
avgGS = (gs4(1:itsPlot, 2) + gs5(1:itsPlot, 2) + gs6(1:itsPlot, 2)) / 3;

figure(1)
plot(gs4(1:itsPlot, 1), gs4(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel 4', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

figure(2)
plot(gs5(1:itsPlot, 1), gs5(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel 5', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

figure(3)
plot(gs6(1:itsPlot, 1), gs6(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel 6', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

figure(4)
plot(iter, avgGS)
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel, 3 times average', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

%% Jacobi
clf; close all;
load('j4.txt')
load('j5.txt')
load('j6.txt')

iter = j4(1:itsPlot, 1);
avgJ = (j4(1:itsPlot, 2) + j5(1:itsPlot, 2) + j6(1:itsPlot, 2)) / 3;

figure(1)
plot(j4(1:itsPlot, 1), j4(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi 4', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

figure(2)
plot(j5(1:itsPlot, 1), j5(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi 5', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

figure(3)
plot(j6(1:itsPlot, 1), j6(1:itsPlot, 2));
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi 6', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

figure(4)
plot(iter, avgJ)
xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Jacobi, 3 times average', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

%% GS v Jacobi
% !Requires both sections above to be run first
clf; close all;

hold on
plot(iter, avgJ)
plot(iter, avgGS)

xlabel('Iteration', 'Interpreter', 'LaTeX')
ylabel('Sum of constraint residuals', 'Interpreter', 'LaTeX')
title('Gauss-Seidel v Jacobi, 3 times average', 'Interpreter', 'LaTeX')
set(gca, 'TickLabelInterpreter', 'LaTeX')
axis([0 itsPlot 0 3500])

xvertsJ = [iter(1:end-1); flipud(iter(2:end)); iter(1)];
yvertsJ = [zeros(itsPlot-1, 1); flipud(avgJ(2:end)); avgJ(1)];
pJ = patch(xvertsJ, yvertsJ, 'b', 'FaceAlpha', 0.6, 'LineStyle', 'none');

xvertsGS = [iter(1:end-1); flipud(iter(2:end)); iter(1)];
yvertsGS = [zeros(itsPlot-1, 1); flipud(avgGS(2:end)); avgGS(1)];
pGS = patch(xvertsGS, yvertsGS, 'r', 'FaceAlpha', 0.6, 'LineStyle', 'none');

cTotJ = trapz(avgJ)
cTotGS = trapz(avgGS)

relC = cTotJ/cTotGS

dim = [0.55 0.4 0.5 0.3];
str = sprintf('GS: int(sum(c)) = %2.3d\nJ: int(sum(c)) = %2.3d\nJ/GS = %1.3f', cTotGS, cTotJ, relC);
annotation('textbox', dim, 'String', str, 'FitBoxToText', 'on', 'Interpreter', 'LaTeX')

legend({'Jacobi', 'Gauss-Seidel', 'Jacobi', 'Gauss-Seidel'}, 'Interpreter', 'LaTeX')
