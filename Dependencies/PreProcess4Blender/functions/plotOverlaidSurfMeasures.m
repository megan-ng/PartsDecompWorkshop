function fig = plotOverlaidSurfMeasures(mesh, measures, sampleName, T_clim)
    % Get screen size for reference
    screenSize = get(0, 'ScreenSize');
    
    % Determine base width and height based on screen size and content
    baseWidth = max(300, (screenSize(3) * 0.75) / max(length(measures), 3)); % Set a min width of 300
    baseHeight = min(400, screenSize(4) * 0.6); % Set height as 60% of screen height, up to 400 px
    
    % Calculate total figure width and set position at center of screen
    totalWidth = baseWidth * length(measures);
    posX = (screenSize(3) - totalWidth) / 2;
    posY = (screenSize(4) - baseHeight) / 2;

    % Create figure with calculated size and centered position
    fig = figure('Position', [posX, posY, totalWidth, baseHeight]);

    % Set up tiled layout with compact spacing
    t = tiledlayout(1, length(measures), 'TileSpacing', 'compact', 'Padding', 'compact');
    for i = 1:length(measures)
        nexttile;
        patch(mesh, 'EdgeColor', 'none', 'FaceVertexCData', measures(i).values, 'FaceColor', 'interp');
        daspect([1 1 1]); % Maintain equal aspect ratio
        title(measures(i).name);
        colorbar;
        % clim([0 1]);
        % 
        % % change colour bar limits if needed for visualization purposes
        % if measures(i).name == "Thinness"
        %      clim(T_clim);
        % end
       
        axis off;
    end
    title(t, sampleName, "Interpreter", "none", "fontweight", "bold");
end


